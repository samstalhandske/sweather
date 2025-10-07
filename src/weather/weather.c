#include "weather.h"

#include <assert.h>
#include "score/string/string.h"
#include "score/buffer/writer/writer.h"

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

    Score_Http_Response response = {0};

    char url[1024];
    unsigned int i;
    for(i = 0; i < sizeof(url); i++) {
        url[i] = '\0';
    }

    Score_Buffer url_buffer = score_buffer_create_from_backing(&url[0], sizeof(url));
    Score_Buffer_Writer writer = score_buffer_writer_create(&url_buffer);

    open_meteo_write_url_to_writer(&writer, data_request);

    printf("Url: '%s'.\n", url);

    if(!score_http_perform(&retriever->http, url, &response)) {
        return false;
    }

    printf("Response: '%s'\n", response.data);

    score_http_dispose_response(&response);

    return true;
}

