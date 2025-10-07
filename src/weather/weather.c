#include "weather.h"

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
    assert(retriever != NULL);
    assert(out_report != NULL);

    printf("Getting weather-report from %f° N, %f° E ...\n", data_request->coordinate.latitude, data_request->coordinate.longitude);

    SCore_Http_Response response = {0};

    char url_backing[1024];
    {
        unsigned int i;
        for(i = 0; i < sizeof(url_backing); i++) {
            url_backing[i] = '\0';
        }
    }
    SCore_Buffer url_buffer = score_buffer_create_from_backing(&url_backing[0], sizeof(url_backing));
    SCore_Buffer_Writer url_writer = score_buffer_writer_create(&url_buffer);

    open_meteo_write_url_to_writer(&url_writer, data_request);

    printf("Url: '%s'.\n", url_backing);

    if(!score_http_perform(&retriever->http, url_backing, &response)) {
        return false;
    }

    printf("Response: '%s'\n", response.data);

    SCore_JSON_Object json_object;
    if(!score_json_parse(response.data, &json_object)) {
        score_http_dispose_response(&response);
        return false;
    }

    char json_buffer_backing[1024];
    {
        unsigned int i;
        for(i = 0; i < sizeof(json_buffer_backing); i++) {
            json_buffer_backing[i] = '\0';
        }
    }

    SCore_Buffer json_buffer = score_buffer_create_from_backing(&json_buffer_backing[0], sizeof(json_buffer_backing));
    SCore_Buffer_Writer json_writer = score_buffer_writer_create(&json_buffer);
    if(!score_json_write_to_buffer(&json_writer, json_object)) {
        score_http_dispose_response(&response);
        return false;
    }

    /* printf("Buffer: %s\n", score_buffer_to_string(json_buffer)); */
    printf("Json Buffer: '%s'.\n", json_buffer_backing);

    score_http_dispose_response(&response);
    return true;
}