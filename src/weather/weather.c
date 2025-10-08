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
    char json_buffer_backing[4096];
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
        char url_backing[1024];
        SCore_Buffer url_buffer;
        SCore_Buffer_Writer url_writer;

        for (i = 0; i < sizeof(url_backing); i++) {
            url_backing[i] = '\0';
        }

        url_buffer = score_buffer_create_from_backing(&url_backing[0], sizeof(url_backing));
        url_writer = score_buffer_writer_create(&url_buffer);

        open_meteo_write_url_to_writer(&url_writer, data_request);

        printf("Url: '%s'.\n", url_backing);

        if (!score_http_perform(&retriever->http, url_backing, &response)) {
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

    if (!open_meteo_create_report_from_json_object(&json_object, &open_meteo_report)) {
        score_json_dispose(&json_object);
        score_http_dispose_response(&response);
        return false;
    }

    open_meteo_report.current.flags = data_request->current_flags;
    open_meteo_report.hourly.flags  = data_request->hourly_flags;
    open_meteo_report.daily.flags   = data_request->daily_flags;

    printf("## Open_Meteo_Report\n");
    printf("Current:\n");
    {
        Open_Meteo_Report_Current *v = &open_meteo_report.current;
        printf("- Flags: %lu.\n", v->flags);
    }
    printf("Hourly:\n");
    {
        Open_Meteo_Report_Hourly *v = &open_meteo_report.hourly;
        printf("- Flags: %lu.\n", v->flags);
        printf("- Entry-count: %u.\n", v->entry_count);
    }
    printf("Daily:\n");
    {
        Open_Meteo_Report_Daily *v = &open_meteo_report.daily;
        printf("- Flags: %lu.\n", v->flags);
        printf("- Entry-count: %u.\n", v->entry_count);
    }

    score_json_dispose(&json_object);
    score_http_dispose_response(&response);

    return true;
}
