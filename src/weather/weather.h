#ifndef WEATHER_H
#define WEATHER_H

#include <stdbool.h>
#include "score/http/http.h"
#include "score/utils/flag/flag.h"

#include "score/integrations/open_meteo/open_meteo.h"

typedef struct {
    SCore_Http http;
} Weather_Retriever;

bool weather_init_retriever(Weather_Retriever *retriever);
void weather_dispose_retriever(Weather_Retriever *retriever);

typedef struct {
    Open_Meteo_Report open_meteo_report;
} Weather_Report;

bool weather_get_report(Weather_Retriever *retriever, const Open_Meteo_Data_Request *data_request, Weather_Report *out_report);

#endif