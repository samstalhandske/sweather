#ifndef WEATHER_H
#define WEATHER_H

#include "score/http/http.h"
#include "score/utils/flag/flag.h"

#include "open_meteo.h"

typedef struct {
    Score_Http http;
} Weather_Retriever;

bool weather_init_retriever(Weather_Retriever *retriever);
void weather_dispose_retriever(Weather_Retriever *retriever);

typedef struct {
    double temperature;
} Weather_Report;

bool weather_get_report(Weather_Retriever *retriever, const Open_Meteo_Data_Request *data_request, Weather_Report *out_report);

#endif