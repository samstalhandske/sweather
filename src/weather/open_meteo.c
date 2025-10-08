#include "open_meteo.h"

#include "score/string/string.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *OPENMETEO_HOURLY_STRINGS[OPENMETEO_HOURLY_FLAG_MAX] = { /* NOTE: SS - ',' required at the moment. */
    "temperature_2m,",
    "relative_humidity_2m,",
    "dewpoint_2m,",
    "apparent_temperature,",
    "precipitation_probability,",
    "precipitation,",
    "rain,",
    "showers,",
    "snowfall,",
    "snow_depth,",
    "weather_code,",
    "pressure_msl,",
    "surface_pressure,",
    "cloud_cover,",
    "cloud_cover_low,",
    "cloud_cover_mid,",
    "cloud_cover_high,",
    "visibility,",
    "evapotranspiration,",
    "et0_fao_evapotranspiration,",
    "vapour_pressure_deficit,",
    "wind_speed_10m,",
    "wind_speed_80m,",
    "wind_speed_120m,",
    "wind_speed_180m,",
    "wind_direction_10m,",
    "wind_direction_80m,",
    "wind_direction_120m,",
    "wind_direction_180m,",
    "wind_gusts_10m,",
    "temperature_80m,",
    "temperature_120m,",
    "temperature_180m,",
    "soil_temperature_0cm,",
    "soil_temperature_6cm,",
    "soil_temperature_18cm,",
    "soil_temperature_54cm,",
    "soil_moisture_0_to_1cm,",
    "soil_moisture_1_to_3cm,",
    "soil_moisture_3_to_9cm,",
    "soil_moisture_9_to_27cm,",
    "soil_moisture_27_to_81cm,"
};

static bool open_meteo_write_if_flag_set(SCore_Buffer_Writer *writer, const uint64_t all_flags, const uint64_t flag, const uint64_t flag_index) {
    if(score_flag64_is_set(all_flags, flag)) {
        if(!score_buffer_writer_write_string(writer, OPENMETEO_HOURLY_STRINGS[flag_index])) {
            return false;
        }
    }

    return true;
}

void open_meteo_write_url_to_writer(SCore_Buffer_Writer *writer, const Open_Meteo_Data_Request *data_request) {
    assert(score_string_snprintf(writer,
        "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f"
        "&forecast_days=%u&past_days=%u"
        "&temperature_unit=%s",
        data_request->coordinate.latitude, data_request->coordinate.longitude,
        data_request->forecast_days, data_request->past_days,
        data_request->temperature_unit == Open_Meteo_Data_Request_Temperature_Unit_Celsius ? "celsius" : "fahrenheit"
    ));

    { /* Hourly. */
        uint64_t flags = data_request->hourly_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&hourly=")) {
                return;
            }

            {
                u64 i;
                for (i = 0; i < OPENMETEO_HOURLY_FLAG_MAX; i++) {
                    uint64_t flag = ((uint64_t)1 << i);
                    if (!open_meteo_write_if_flag_set(writer, flags, flag, i)) {
                        return;
                    }
                }
            }
        }
    }

    { /* Daily. */
        uint64_t flags = data_request->daily_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&daily=")) {
                return;
            }
        }


    }

    { /* Current. */
        uint64_t flags = data_request->current_flags;

        if(score_has_any_flag_set(flags)) {
            if(!score_buffer_writer_write_string(writer, "&current=")) {
                return;
            }
        }


    }
}

SCORE_BOOL open_meteo_create_report_from_json_object(const SCore_JSON_Object *json_object, Open_Meteo_Report *out_report) {
    Open_Meteo_Report_Current report_current;
    Open_Meteo_Report_Hourly report_hourly;
    Open_Meteo_Report_Daily report_daily;

    if(json_object == NULL) {
        return SCORE_FALSE;
    }
    if(out_report == NULL) {
        return SCORE_FALSE;
    }

    /* Current. */
    printf("Current ...\n");
    memset(&report_current, 0, sizeof(Open_Meteo_Report_Current));

    /* Hourly. */
    printf("Hourly ...\n");
    memset(&report_hourly, 0, sizeof(report_hourly));

    {
        SCore_JSON_Object hourly_object;
        if(!score_json_get_object(json_object, "hourly", SCORE_FALSE, &hourly_object)) {
            return false;
        }

        {
            SCore_JSON_Object time_object;
            SCore_JSON_Array time_array;

            if(!score_json_get_object(&hourly_object, "time", SCORE_FALSE, &time_object)) {
                return false;
            }

            if(score_json_as_array(&time_object, &time_array)) {
                uint32_t i;

                printf("Time array contains %u elements.\n", time_array.size);

                /* Allocate 'time_array.size' elements of Open_Meteo_Report_Hourly_Entries. */
                report_hourly.entry_count = time_array.size;
                report_hourly.entries = calloc(report_hourly.entry_count, sizeof(Open_Meteo_Report_Hourly_Entry));
                assert(report_hourly.entries != NULL);

                for(i = 0; i < report_hourly.entry_count; i++) {
                    Open_Meteo_Report_Hourly_Entry *entry = &report_hourly.entries[i];
                    assert(entry != NULL);

                    assert(json_to_date_time(&time_array.data[i], &entry->date_time));

                    printf("Date_Time for element %u:\n", i);
                    print_date(entry->date_time.date);
                    print_time(entry->date_time.time);
                }

                /*
                char backing[64];
                memset(&backing[0], 0, sizeof(backing));
                SCore_Buffer buf = score_buffer_create_from_backing(&backing[0], sizeof(backing));

                uint32_t i;
                for(i = 0; i < time_array.size; i++) {
                    memset(&backing[0], 0, sizeof(backing));
                    SCore_Buffer_Writer writer = score_buffer_writer_create(&buf);

                    SCore_JSON_Object *json_object = &time_array.data[i];

                    if(!score_json_write_to_buffer(&writer, *json_object)) {
                        break;
                    }

                    printf("Element %u: '%s'.\n", i, backing);
                }
                */
            }

            score_json_array_dispose(&time_array);
        }
    }


    /* Daily. */
    memset(&report_daily, 0, sizeof(Open_Meteo_Report_Daily));

    out_report->current  = report_current;
    out_report->hourly   = report_hourly;
    out_report->daily    = report_daily;

    return SCORE_TRUE;
}