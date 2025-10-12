#include "weather.h"

#include <string.h>
#include <assert.h>
#include "score/string/string.h"
#include "score/buffer/writer/writer.h"
#include "score/json/json.h"

#include "open_meteo.h"

bool weather_init_retriever(Weather_Retriever *retriever) {
    assert(retriever != NULL);
    assert(retriever->http.curl == NULL);

    return score_http_initialize(&retriever->http);
}

void weather_dispose_retriever(Weather_Retriever *retriever) {
    assert(retriever != NULL);
    assert(retriever->http.curl != NULL);

    score_http_dispose(&retriever->http);
}

bool weather_get_report(Weather_Retriever *retriever, const Open_Meteo_Data_Request *data_request, Weather_Report *out_report) {
    SCore_Http_Response response;
    SCore_JSON_Object json_object;
    uint8_t json_buffer_backing[4096];
    SCore_Buffer json_buffer;
    SCore_Buffer_Writer json_writer;
    Open_Meteo_Report open_meteo_report;
    unsigned int i;

    assert(retriever != NULL);
    assert(out_report != NULL);

    printf("Getting weather-report from %f° N, %f° E ...\n",
           data_request->coordinate.latitude,
           data_request->coordinate.longitude);

    memset(&response, 0, sizeof(SCore_Http_Response));
    {
        uint8_t url_backing[1024];
        SCore_Buffer url_buffer;
        SCore_Buffer_Writer url_writer;

        for (i = 0; i < sizeof(url_backing); i++) {
            url_backing[i] = '\0';
        }

        url_buffer = score_buffer_create_from_backing(&url_backing[0], sizeof(url_backing));
        url_writer = score_buffer_writer_create(&url_buffer);

        open_meteo_write_url_to_writer(&url_writer, data_request);

        printf("Url: '%s'.\n", url_backing);

        if (!score_http_perform(&retriever->http, (char *)url_backing, &response)) {
            return false;
        }
    }

    if (!score_json_parse(response.data, &json_object)) {
        score_http_dispose_response(&response);
        return false;
    }

    for (i = 0; i < sizeof(json_buffer_backing); i++) {
        json_buffer_backing[i] = '\0';
    }

    json_buffer = score_buffer_create_from_backing(&json_buffer_backing[0],
                                                   sizeof(json_buffer_backing));
    json_writer = score_buffer_writer_create(&json_buffer);

    if (!score_json_write_to_buffer(&json_writer, json_object)) {
        score_json_dispose(&json_object);
        score_http_dispose_response(&response);
        return false;
    }

    printf("Json Buffer: '%s'.\n", json_buffer_backing);

    if (!open_meteo_create_report_from_json_object(
            &json_object,
            data_request->current_flags,
            data_request->hourly_flags,
            data_request->daily_flags,
            &open_meteo_report)) {
        score_json_dispose(&json_object);
        score_http_dispose_response(&response);
        return false;
    }

    printf("## Open_Meteo_Report\n");
    printf("Time-zone abbreviation: %s.\n", open_meteo_report.timezone_abbreviation);
    printf("Current:\n");
    {
        Open_Meteo_Report_Current *v = &open_meteo_report.current;
        printf("- Flags: %u.\n", v->flags);
    }
    printf("Hourly:\n");
    {
        Open_Meteo_Report_Hourly *v = &open_meteo_report.hourly;
        /*printf("- Flags: %u.\n", v->flags);*/
        printf("- Entry-count: %u.\n", v->entry_count);
        {
            uint32_t i;
            for(i = 0; i < v->entry_count; i++) {
                Open_Meteo_Report_Hourly_Entry *entry = &v->entries[i];

                printf("(%02i) %02u-%02u-%02u %02u:%02u:%02u\n",
                    i, entry->date_time.date.year, entry->date_time.date.month, entry->date_time.date.day, entry->date_time.time.hour, entry->date_time.time.minute, entry->date_time.time.second
                );

                { /* Temperatures.*/
                    if(entry->temperature_2m.is_set) {
                        Open_Meteo_Report_Temperature *temp = &entry->temperature_2m;
                        printf("* temperature (%um): %.1f %s.\n",
                            temp->meters_above_ground,
                            temp->temperature,
                            temp->unit == Open_Meteo_Unit_Temperature_Celsius ? "°C" : "°F"
                        );
                    }
                    if(entry->temperature_80m.is_set) {
                        Open_Meteo_Report_Temperature *temp = &entry->temperature_80m;
                        printf("* temperature (%um): %.1f %s.\n",
                            temp->meters_above_ground,
                            temp->temperature,
                            temp->unit == Open_Meteo_Unit_Temperature_Celsius ? "°C" : "°F"
                        );
                    }
                    if(entry->temperature_120m.is_set) {
                        Open_Meteo_Report_Temperature *temp = &entry->temperature_120m;
                        printf("* temperature (%um): %.1f %s.\n",
                            temp->meters_above_ground,
                            temp->temperature,
                            temp->unit == Open_Meteo_Unit_Temperature_Celsius ? "°C" : "°F"
                        );
                    }
                    if(entry->temperature_180m.is_set) {
                        Open_Meteo_Report_Temperature *temp = &entry->temperature_180m;
                        printf("* temperature (%um): %.1f %s.\n",
                            temp->meters_above_ground,
                            temp->temperature,
                            temp->unit == Open_Meteo_Unit_Temperature_Celsius ? "°C" : "°F"
                        );
                    }
                    if(entry->apparent_temperature.is_set) {
                        Open_Meteo_Report_Temperature *temp = &entry->apparent_temperature;
                        printf("* apparent_temperature: %.1f %s.\n",
                            temp->temperature,
                            temp->unit == Open_Meteo_Unit_Temperature_Celsius ? "°C" : "°F"
                        );
                    }
                }
                { /* Rain. */
                    if(entry->rain.is_set) {
                        Open_Meteo_Report_Rain *v = &entry->rain;
                        printf("* rain: %.1f %s.\n",
                            v->value,
                            v->unit == Open_Meteo_Unit_Rain_Millimeter ? "mm" : "??"
                        );
                    }
                }


                if(entry->date_time.time.hour % 23 == 0 && entry->date_time.time.hour != 0) {
                    printf("\n");
                }
            }
        }
    }
    printf("Daily:\n");
    {
        Open_Meteo_Report_Daily *v = &open_meteo_report.daily;
        printf("- Flags: %u.\n", v->flags);
        printf("- Entry-count: %u.\n", v->entry_count);
    }

    score_json_dispose(&json_object);
    score_http_dispose_response(&response);

    return true;
}
