#ifndef OPEN_METEO_FLAGS_H
#define OPEN_METEO_FLAGS_H

/* https://open-meteo.com/en/docs */

#include "score/buffer/writer/writer.h"
#include "score/utils/flag/flag.h"
#include "score/json/json.h"

#include "coordinate.h"
#include "date.h"

typedef enum {
    Open_Meteo_Unit_Temperature_Celsius,
    Open_Meteo_Unit_Temperature_Fahrenheit
} Open_Meteo_Unit_Temperature;

typedef enum {
    Open_Meteo_Unit_Rain_Millimeter
} Open_Meteo_Unit_Rain;

typedef struct {
    bool temperature_2m;
    bool temperature_80m;
    bool temperature_120m;
    bool temperature_180m;
    bool relative_humidity_2m;
    bool dewpoint_2m;
    bool apparent_temperature;
    bool precipitation_probability;
    bool precipitation;
    bool rain;
    bool showers;
    bool snowfall;
    bool snow_depth;
    bool weather_code;
    bool sealevel_pressure;
    bool surface_pressure;
    bool cloud_cover_total;
    bool cloud_cover_low;
    bool cloud_cover_mid;
    bool cloud_cover_high;
    bool visibility;
    bool evapotranspiration;
    bool reference_evapotranspiration;
    bool vapour_pressure_deficit;
    bool wind_speed_10m;
    bool wind_speed_80m;
    bool wind_speed_120m;
    bool wind_speed_180m;
    bool wind_direction_10m;
    bool wind_direction_80m;
    bool wind_direction_120m;
    bool wind_direction_180m;
    bool wind_gusts_10m;
    bool soil_temperature_0cm;
    bool soil_temperature_6cm;
    bool soil_temperature_18cm;
    bool soil_temperature_54cm;
    bool soil_moisture_0_1cm;
    bool soil_moisture_1_3cm;
    bool soil_moisture_3_9cm;
    bool soil_moisture_9_27cm;
    bool soil_moisture_27_81cm;
} Open_Meteo_Hourly_Flags;

typedef struct {
    Geographical_Coordinate coordinate;
    unsigned char past_days;
    unsigned char forecast_days;

    Open_Meteo_Unit_Temperature temperature_unit;

    SCore_Flags current_flags;
    Open_Meteo_Hourly_Flags hourly_flags;
    SCore_Flags daily_flags;

} Open_Meteo_Data_Request;

typedef struct {
    bool is_set;
    Open_Meteo_Unit_Temperature unit; /* Kind of unnecessary cause now every temperature instance will have this with the same value. */
    f32 temperature;
    uint8_t meters_above_ground;
} Open_Meteo_Report_Temperature;

typedef struct {
    bool is_set;
    Open_Meteo_Unit_Rain unit;
    f32 value;
} Open_Meteo_Report_Rain;

typedef struct {
    Date_Time date_time;

    Open_Meteo_Report_Temperature apparent_temperature;

    Open_Meteo_Report_Temperature temperature_2m;
    Open_Meteo_Report_Temperature temperature_80m;
    Open_Meteo_Report_Temperature temperature_120m;
    Open_Meteo_Report_Temperature temperature_180m;

    Open_Meteo_Report_Rain rain;
} Open_Meteo_Report_Hourly_Entry;

typedef struct {
    Date date;
} Open_Meteo_Report_Daily_Entry;

/* TODO: SS - Parse the current/hourly/daily_units. */

typedef struct {
    Date_Time date_time;
    uint32_t flags;
} Open_Meteo_Report_Current;

typedef struct {
    Open_Meteo_Hourly_Flags flags;
    Open_Meteo_Report_Hourly_Entry* entries;
    uint32_t entry_count;

} Open_Meteo_Report_Hourly;

typedef struct {
    Open_Meteo_Report_Daily_Entry* entries;
    uint32_t entry_count;
    uint32_t flags;
} Open_Meteo_Report_Daily;


typedef struct {
    uint8_t timezone_abbreviation[8];

    Open_Meteo_Report_Current current;
    Open_Meteo_Report_Hourly hourly;
    Open_Meteo_Report_Daily daily;
} Open_Meteo_Report;

void open_meteo_write_url_to_writer(SCore_Buffer_Writer *writer, const Open_Meteo_Data_Request *data_request);

bool open_meteo_create_report_from_json_object(const SCore_JSON_Object *json_object, uint32_t current_flags, Open_Meteo_Hourly_Flags hourly_flags, uint32_t daily_flags, Open_Meteo_Report *out_report);

/* Hourly. */
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_2M                  ((uint32_t)1 << 0)
#define OPENMETEO_HOURLY_FLAG_RELATIVE_HUMIDITY_2M            ((uint32_t)1 << 1)
#define OPENMETEO_HOURLY_FLAG_DEWPOINT_2M                     ((uint32_t)1 << 2)
#define OPENMETEO_HOURLY_FLAG_APPARENT_TEMPERATURE            ((uint32_t)1 << 3)
#define OPENMETEO_HOURLY_FLAG_PRECIPITATION_PROBABILITY       ((uint32_t)1 << 4)
#define OPENMETEO_HOURLY_FLAG_PRECIPITATION                   ((uint32_t)1 << 5)
#define OPENMETEO_HOURLY_FLAG_RAIN                            ((uint32_t)1 << 6)
#define OPENMETEO_HOURLY_FLAG_SHOWERS                         ((uint32_t)1 << 7)
#define OPENMETEO_HOURLY_FLAG_SNOWFALL                        ((uint32_t)1 << 8)
#define OPENMETEO_HOURLY_FLAG_SNOW_DEPTH                      ((uint32_t)1 << 9)
#define OPENMETEO_HOURLY_FLAG_WEATHER_CODE                    ((uint32_t)1 << 10)
#define OPENMETEO_HOURLY_FLAG_SEALEVEL_PRESSURE               ((uint32_t)1 << 11)
#define OPENMETEO_HOURLY_FLAG_SURFACE_PRESSURE                ((uint32_t)1 << 12)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_TOTAL               ((uint32_t)1 << 13)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_LOW                 ((uint32_t)1 << 14)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_MID                 ((uint32_t)1 << 15)
#define OPENMETEO_HOURLY_FLAG_CLOUD_COVER_HIGH                ((uint32_t)1 << 16)
#define OPENMETEO_HOURLY_FLAG_VISIBILITY                      ((uint32_t)1 << 17)
#define OPENMETEO_HOURLY_FLAG_EVAPOTRANSPIRATION              ((uint32_t)1 << 18)
#define OPENMETEO_HOURLY_FLAG_REFERENCE_EVAPOTRANSPIRATION    ((uint32_t)1 << 19)
#define OPENMETEO_HOURLY_FLAG_VAPOUR_PRESSURE_DEFICIT         ((uint32_t)1 << 20)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_10M                  ((uint32_t)1 << 21)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_80M                  ((uint32_t)1 << 22)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_120M                 ((uint32_t)1 << 23)
#define OPENMETEO_HOURLY_FLAG_WIND_SPEED_180M                 ((uint32_t)1 << 24)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_10M              ((uint32_t)1 << 25)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_80M              ((uint32_t)1 << 26)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_120M             ((uint32_t)1 << 27)
#define OPENMETEO_HOURLY_FLAG_WIND_DIRECTION_180M             ((uint32_t)1 << 28)
#define OPENMETEO_HOURLY_FLAG_WIND_GUSTS_10M                  ((uint32_t)1 << 29)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_80M                 ((uint32_t)1 << 30)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_120M                ((uint32_t)1 << 31)
#define OPENMETEO_HOURLY_FLAG_TEMPERATURE_180M                ((uint32_t)1 << 32)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_0CM            ((uint32_t)1 << 33)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_6CM            ((uint32_t)1 << 34)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_18CM           ((uint32_t)1 << 35)
#define OPENMETEO_HOURLY_FLAG_SOIL_TEMPERATURE_54CM           ((uint32_t)1 << 36)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_0_1CM             ((uint32_t)1 << 37)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_1_3CM             ((uint32_t)1 << 38)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_3_9CM             ((uint32_t)1 << 39)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_9_27CM            ((uint32_t)1 << 40)
#define OPENMETEO_HOURLY_FLAG_SOIL_MOISTURE_27_81CM           ((uint32_t)1 << 41)
#define OPENMETEO_HOURLY_FLAG_MAX                             42

#endif /* OPEN_METEO_FLAGS_H */