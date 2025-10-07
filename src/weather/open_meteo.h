#ifndef OPEN_METEO_FLAGS_H
#define OPEN_METEO_FLAGS_H

/* https://open-meteo.com/en/docs */

#include "score/buffer/writer/writer.h"
#include "score/utils/flag/flag.h"
#include "coordinate.h"

typedef struct {
    Geographical_Coordinate coordinate;
    unsigned char forecast_days;

    uint64_t current_flags;
    uint64_t hourly_flags;
    uint64_t daily_flags;
} Open_Meteo_Data_Request;

typedef enum {
    Open_Meteo_Type_Hourly,
} Open_Meteo_Type;

void open_meteo_write_url_to_writer(SCore_Buffer_Writer *writer, const Open_Meteo_Data_Request *data_request);

/* Hourly. */
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_2M                  ((uint64_t)1 << 0)
#define OPENMETEO_HOURLY_FLAG_RELATIVE_HUMIDITY_2M            ((uint64_t)1 << 1)
#define OPENMETEO_HOURLY_FLAG_DEWPOINT_2M                     ((uint64_t)1 << 2)
#define OPENMETEO_HOURLY_FLAG_APPARENT_TEMPERATURE            ((uint64_t)1 << 3)
#define OPENMETEO_HOURLY_FLAG_PRECIPITATION_PROBABILITY       ((uint64_t)1 << 4)
#define OPENMETEO_HOURLY_FLAG_PRECIPITATION                   ((uint64_t)1 << 5)
#define OPENMETEO_HOURLY_FLAG_RAIN                            ((uint64_t)1 << 6)
#define OPENMETEO_HOURLY_FLAG_SHOWERS                         ((uint64_t)1 << 7)
#define OPENMETEO_HOURLY_FLAG_SNOWFALL                        ((uint64_t)1 << 8)
#define OPENMETEO_HOURLY_FLAG_SNOW_DEPTH                      ((uint64_t)1 << 9)
#define OPENMETEO_HOURLY_FLAG_WEATHER_CODE                    ((uint64_t)1 << 10)
#define OPENMETEO_HOURLY_FLAG_SEALEVEL_PRESSURE               ((uint64_t)1 << 11)
#define OPENMETEO_HOURLY_FLAG_SURFACE_PRESSURE                ((uint64_t)1 << 12)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_TOTAL               ((uint64_t)1 << 13)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_LOW                 ((uint64_t)1 << 14)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_MID                 ((uint64_t)1 << 15)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_HIGH                ((uint64_t)1 << 16)
#define OPENMETEO_HOURLY_FLAG_VISIBILITY                      ((uint64_t)1 << 17)
#define OPENMETEO_HOURLY_FLAG_EVAPOTRANSPIRATION              ((uint64_t)1 << 18)
#define OPENMETEO_HOURLY_FLAG_REFERENCE_EVAPOTRANSPIRATION    ((uint64_t)1 << 19)
#define OPENMETEO_HOURLY_FLAG_VAPOUR_PRESSURE_DEFICIT         ((uint64_t)1 << 20)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_10M                  ((uint64_t)1 << 21)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_80M                  ((uint64_t)1 << 22)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_120M                 ((uint64_t)1 << 23)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_180M                 ((uint64_t)1 << 24)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_10M              ((uint64_t)1 << 25)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_80M              ((uint64_t)1 << 26)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_120M             ((uint64_t)1 << 27)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_180M             ((uint64_t)1 << 28)
#define OPENMETEO_HOURLY_FLAG_WIND_GUSTS_10M                  ((uint64_t)1 << 29)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_80M                 ((uint64_t)1 << 30)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_120M                ((uint64_t)1 << 31)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_180M                ((uint64_t)1 << 32)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_0CM            ((uint64_t)1 << 33)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_6CM            ((uint64_t)1 << 34)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_18CM           ((uint64_t)1 << 35)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_54CM           ((uint64_t)1 << 36)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_0_1CM             ((uint64_t)1 << 37)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_1_3CM             ((uint64_t)1 << 38)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_3_9CM             ((uint64_t)1 << 39)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_9_27CM            ((uint64_t)1 << 40)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_27_81CM           ((uint64_t)1 << 41)
#define OPENMETEO_HOURLY_FLAG_MAX                             42

#endif /* OPEN_METEO_FLAGS_H */