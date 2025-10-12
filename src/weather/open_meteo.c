#include "open_meteo.h"

#include "score/string/string.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool save_apparent_temperature_to_report(Open_Meteo_Report_Hourly *report, const SCore_JSON_Object *parent_to_temperature_object, const SCore_JSON_Object *units_object, const char *name) {
    SCore_JSON_Object obj;
    SCore_JSON_Array arr;

    assert(score_json_get_object(parent_to_temperature_object, name, SCORE_FALSE, &obj));
    assert(score_json_as_array(&obj, &arr));

    {
        uint32_t i;
        SCore_JSON_Object unit_object;
        char *unit_text = NULL;
        Open_Meteo_Unit_Temperature unit;
        printf("Setting entries' %s.\n", name);

        assert(score_json_get_object(units_object, name, SCORE_FALSE, &unit_object));
        assert(score_json_as_string(&unit_object, &unit_text));

        if(score_string_compare(unit_text, "°F", false) == SCore_String_Compare_Result_Equal) {
            unit = Open_Meteo_Unit_Temperature_Fahrenheit;
        }
        else if(score_string_compare(unit_text, "°C", false) == SCore_String_Compare_Result_Equal) {
            unit = Open_Meteo_Unit_Temperature_Celsius;
        }
        else {
            assert(false);
        }

        for(i = 0; i < report->entry_count; i++) {
            Open_Meteo_Report_Hourly_Entry *entry = &report->entries[i];
            Open_Meteo_Report_Temperature *temp = &entry->apparent_temperature;
            double temperature = 0;
            assert(temp != NULL);

            assert(score_json_as_number(&arr.data[i], &temperature));

            temp->is_set = true;
            temp->unit = unit;
            temp->temperature = (f32)temperature;
            temp->meters_above_ground = 0;
        }
    }

    score_json_array_dispose(&arr);
    return true;
}

bool save_temperature_to_report(Open_Meteo_Report_Hourly *report, const SCore_JSON_Object *parent_to_temperature_object, const SCore_JSON_Object *units_object, const char *name, uint8_t meters_above_ground) {
    SCore_JSON_Object obj;
    SCore_JSON_Array arr;

    assert(score_json_get_object(parent_to_temperature_object, name, SCORE_FALSE, &obj));
    assert(score_json_as_array(&obj, &arr));

    {
        uint32_t i;
        SCore_JSON_Object unit_object;
        char *unit_text = NULL;
        Open_Meteo_Unit_Temperature unit;
        printf("Setting entries' %s.\n", name);

        assert(score_json_get_object(units_object, name, SCORE_FALSE, &unit_object));
        assert(score_json_as_string(&unit_object, &unit_text));

        if(score_string_compare(unit_text, "°F", false) == SCore_String_Compare_Result_Equal) {
            unit = Open_Meteo_Unit_Temperature_Fahrenheit;
        }
        else if(score_string_compare(unit_text, "°C", false) == SCore_String_Compare_Result_Equal) {
            unit = Open_Meteo_Unit_Temperature_Celsius;
        }
        else {
            assert(false);
        }

        for(i = 0; i < report->entry_count; i++) {
            Open_Meteo_Report_Hourly_Entry *entry = &report->entries[i];
            Open_Meteo_Report_Temperature *temp = NULL;
            double temperature = 0;

            assert(score_json_as_number(&arr.data[i], &temperature));

            switch(meters_above_ground) {
                case 2:     { temp = &entry->temperature_2m;    break; }
                case 80:    { temp = &entry->temperature_80m;   break; }
                case 120:   { temp = &entry->temperature_120m;  break; }
                case 180:   { temp = &entry->temperature_180m;  break; }
                default:
                    break;
            }
            assert(temp != NULL);

            temp->is_set = true;
            temp->unit = unit;
            temp->temperature = (f32)temperature;
            temp->meters_above_ground = meters_above_ground;
        }
    }

    score_json_array_dispose(&arr);
    return true;
}

bool save_rain_to_report(Open_Meteo_Report_Hourly *report, const SCore_JSON_Object *parent_to_object, const SCore_JSON_Object *units_object, const char *name) {
    SCore_JSON_Object obj;
    SCore_JSON_Array arr;

    assert(score_json_get_object(parent_to_object, name, SCORE_FALSE, &obj));
    assert(score_json_as_array(&obj, &arr));

    {
        uint32_t i;
        SCore_JSON_Object unit_object;
        char *unit_text = NULL;
        Open_Meteo_Unit_Rain unit;
        printf("Setting entries' %s.\n", name);

        assert(score_json_get_object(units_object, name, SCORE_FALSE, &unit_object));
        assert(score_json_as_string(&unit_object, &unit_text));

        if(score_string_compare(unit_text, "mm", false) == SCore_String_Compare_Result_Equal) {
            unit = Open_Meteo_Unit_Rain_Millimeter;
        }
        else {
            assert(false);
        }

        for(i = 0; i < report->entry_count; i++) {
            Open_Meteo_Report_Hourly_Entry *entry = &report->entries[i];
            Open_Meteo_Report_Rain *temp = &entry->rain;
            double v = 0;

            assert(score_json_as_number(&arr.data[i], &v));

            temp->is_set = true;
            temp->unit = unit;
            temp->value = (f32)v;
        }
    }

    score_json_array_dispose(&arr);
    return true;
}


void open_meteo_write_url_to_writer(SCore_Buffer_Writer *writer, const Open_Meteo_Data_Request *data_request) {
    assert(score_string_snprintf(writer,
        "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f"
        "&forecast_days=%u&past_days=%u"
        "&temperature_unit=%s",
        data_request->coordinate.latitude, data_request->coordinate.longitude,
        data_request->forecast_days, data_request->past_days,
        data_request->temperature_unit == Open_Meteo_Unit_Temperature_Celsius ? "celsius" : "fahrenheit"
    ));

    { /* Hourly. */
        if(score_buffer_writer_write_string(writer, "&hourly=")) {
            Open_Meteo_Hourly_Flags f = data_request->hourly_flags;
            if(f.temperature_2m)                { if(!score_buffer_writer_write_string(writer, "temperature_2m,"))                { return; } }
            if(f.temperature_80m)               { if(!score_buffer_writer_write_string(writer, "temperature_80m,"))               { return; } }
            if(f.temperature_120m)              { if(!score_buffer_writer_write_string(writer, "temperature_120m,"))              { return; } }
            if(f.temperature_180m)              { if(!score_buffer_writer_write_string(writer, "temperature_180m,"))              { return; } }
            if(f.relative_humidity_2m)          { if(!score_buffer_writer_write_string(writer, "relative_humidity_2m,"))          { return; } }
            if(f.dewpoint_2m)                   { if(!score_buffer_writer_write_string(writer, "dewpoint_2m,"))                   { return; } }
            if(f.apparent_temperature)          { if(!score_buffer_writer_write_string(writer, "apparent_temperature,"))          { return; } }
            if(f.precipitation_probability)     { if(!score_buffer_writer_write_string(writer, "precipitation_probability,"))     { return; } }
            if(f.precipitation)                 { if(!score_buffer_writer_write_string(writer, "precipitation,"))                 { return; } }
            if(f.rain)                          { if(!score_buffer_writer_write_string(writer, "rain,"))                          { return; } }
            if(f.showers)                       { if(!score_buffer_writer_write_string(writer, "showers,"))                       { return; } }
            if(f.snowfall)                      { if(!score_buffer_writer_write_string(writer, "snowfall,"))                      { return; } }
            if(f.snow_depth)                    { if(!score_buffer_writer_write_string(writer, "snow_depth,"))                    { return; } }
            if(f.weather_code)                  { if(!score_buffer_writer_write_string(writer, "weather_code,"))                  { return; } }
            if(f.sealevel_pressure)             { if(!score_buffer_writer_write_string(writer, "sealevel_pressure,"))             { return; } }
            if(f.surface_pressure)              { if(!score_buffer_writer_write_string(writer, "surface_pressure,"))              { return; } }
            if(f.cloud_cover_total)             { if(!score_buffer_writer_write_string(writer, "cloud_cover_total,"))             { return; } }
            if(f.cloud_cover_low)               { if(!score_buffer_writer_write_string(writer, "cloud_cover_low,"))               { return; } }
            if(f.cloud_cover_mid)               { if(!score_buffer_writer_write_string(writer, "cloud_cover_mid,"))               { return; } }
            if(f.cloud_cover_high)              { if(!score_buffer_writer_write_string(writer, "cloud_cover_high,"))              { return; } }
            if(f.visibility)                    { if(!score_buffer_writer_write_string(writer, "visibility,"))                    { return; } }
            if(f.evapotranspiration)            { if(!score_buffer_writer_write_string(writer, "evapotranspiration,"))            { return; } }
            if(f.reference_evapotranspiration)  { if(!score_buffer_writer_write_string(writer, "reference_evapotranspiration,"))  { return; } }
            if(f.vapour_pressure_deficit)       { if(!score_buffer_writer_write_string(writer, "vapour_pressure_deficit,"))       { return; } }
            if(f.wind_speed_10m)                { if(!score_buffer_writer_write_string(writer, "wind_speed_10m,"))                { return; } }
            if(f.wind_speed_80m)                { if(!score_buffer_writer_write_string(writer, "wind_speed_80m,"))                { return; } }
            if(f.wind_speed_120m)               { if(!score_buffer_writer_write_string(writer, "wind_speed_120m,"))               { return; } }
            if(f.wind_speed_180m)               { if(!score_buffer_writer_write_string(writer, "wind_speed_180m,"))               { return; } }
            if(f.wind_direction_10m)            { if(!score_buffer_writer_write_string(writer, "wind_direction_10m,"))            { return; } }
            if(f.wind_direction_80m)            { if(!score_buffer_writer_write_string(writer, "wind_direction_80m,"))            { return; } }
            if(f.wind_direction_120m)           { if(!score_buffer_writer_write_string(writer, "wind_direction_120m,"))           { return; } }
            if(f.wind_direction_180m)           { if(!score_buffer_writer_write_string(writer, "wind_direction_180m,"))           { return; } }
            if(f.wind_gusts_10m)                { if(!score_buffer_writer_write_string(writer, "wind_gusts_10m,"))                { return; } }
            if(f.soil_temperature_0cm)          { if(!score_buffer_writer_write_string(writer, "soil_temperature_0cm,"))          { return; } }
            if(f.soil_temperature_6cm)          { if(!score_buffer_writer_write_string(writer, "soil_temperature_6cm,"))          { return; } }
            if(f.soil_temperature_18cm)         { if(!score_buffer_writer_write_string(writer, "soil_temperature_18cm,"))         { return; } }
            if(f.soil_temperature_54cm)         { if(!score_buffer_writer_write_string(writer, "soil_temperature_54cm,"))         { return; } }
            if(f.soil_moisture_0_1cm)           { if(!score_buffer_writer_write_string(writer, "soil_moisture_0_1cm,"))           { return; } }
            if(f.soil_moisture_1_3cm)           { if(!score_buffer_writer_write_string(writer, "soil_moisture_1_3cm,"))           { return; } }
            if(f.soil_moisture_3_9cm)           { if(!score_buffer_writer_write_string(writer, "soil_moisture_3_9cm,"))           { return; } }
            if(f.soil_moisture_9_27cm)          { if(!score_buffer_writer_write_string(writer, "soil_moisture_9_27cm,"))          { return; } }
            if(f.soil_moisture_27_81cm)         { if(!score_buffer_writer_write_string(writer, "soil_moisture_27_81cm,"))         { return; } }
        }
    }

    { /* Daily. */
        SCore_Flags flags = data_request->daily_flags;

        if(score_flag_has_any(flags)) {
            if(score_buffer_writer_write_string(writer, "&daily=")) {
            }
        }


    }

    { /* Current. */
        SCore_Flags flags = data_request->current_flags;

        if(score_flag_has_any(flags)) {
            if(score_buffer_writer_write_string(writer, "&current=")) {

            }
        }


    }
}

static bool create_hourly_report_from_json_object(const SCore_JSON_Object *json_object, Open_Meteo_Hourly_Flags flags, Open_Meteo_Report_Hourly *out_report) {
    SCore_JSON_Object hourly_object;
    SCore_JSON_Object units_object;
    SCore_JSON_Object time_object;
    SCore_JSON_Array time_array;

    if(!score_json_get_object(json_object, "hourly", SCORE_FALSE, &hourly_object)) {
        return SCORE_FALSE;
    }

    if(!score_json_get_object(json_object, "hourly_units", SCORE_FALSE, &units_object)) {
        return SCORE_FALSE;
    }

    assert(score_json_get_object(&hourly_object, "time", SCORE_FALSE, &time_object)); /* We count on finding a 'time' object + array. */
    assert(score_json_as_array(&time_object, &time_array));


    memset(out_report, 0, sizeof(Open_Meteo_Report_Hourly));
    out_report->flags = flags;

    out_report->entry_count = time_array.size;
    assert(out_report->entry_count > 0);
    out_report->entries = calloc(out_report->entry_count, sizeof(Open_Meteo_Report_Hourly_Entry));
    assert(out_report->entries != NULL);


    {
        uint32_t i;
        for(i = 0; i < out_report->entry_count; i++) {
            Open_Meteo_Report_Hourly_Entry *entry = &out_report->entries[i];
            assert(entry != NULL);
            assert(json_to_date_time(&time_array.data[i], &entry->date_time));
        }
    }


    if(flags.apparent_temperature)    assert(save_apparent_temperature_to_report(out_report, &hourly_object, &units_object, "apparent_temperature"));
    if(flags.temperature_2m)    assert(save_temperature_to_report(out_report, &hourly_object, &units_object, "temperature_2m", 2));
    if(flags.temperature_80m)   assert(save_temperature_to_report(out_report, &hourly_object, &units_object, "temperature_80m", 80));
    if(flags.temperature_120m)  assert(save_temperature_to_report(out_report, &hourly_object, &units_object, "temperature_120m", 120));
    if(flags.temperature_180m)  assert(save_temperature_to_report(out_report, &hourly_object, &units_object, "temperature_180m", 180));
    if(flags.rain)              assert(save_rain_to_report(out_report, &hourly_object, &units_object, "rain"));


    /*









    if(flags.temperature_2m) {
        assert(save_temperature_to_report(&hourly_object, &units_object, "temperature_2m", 2));
    }
    if(flags.temperature_80m) {
        SCore_JSON_Object obj;
        SCore_JSON_Array arr;

        assert(score_json_get_object(&hourly_object, "temperature_80m", SCORE_FALSE, &obj));
        assert(score_json_as_array(&obj, &arr));

        {
            uint32_t i;
            SCore_JSON_Object unit_object;
            char *unit_text = NULL;
            Open_Meteo_Unit_Temperature temperature_unit;
            printf("Setting entries' temperature_80m.\n");

            assert(score_json_get_object(&units_object, "temperature_80m", SCORE_FALSE, &unit_object));
            assert(score_json_as_string(&unit_object, &unit_text));

            if(score_string_compare(unit_text, "°F", false) == SCore_String_Compare_Result_Equal) {
                temperature_unit = Open_Meteo_Unit_Temperature_Fahrenheit;
            }
            else if(score_string_compare(unit_text, "°C", false) == SCore_String_Compare_Result_Equal) {
                temperature_unit = Open_Meteo_Unit_Temperature_Celsius;
            }
            else {
                assert(false);
            }

            for(i = 0; i < out_report->entry_count; i++) {
                Open_Meteo_Report_Hourly_Entry *entry = &out_report->entries[i];

                double temperature = 0;
                assert(score_json_as_number(&arr.data[i], &temperature));

                entry->temperature_80m.is_set = true;
                entry->temperature_80m.unit = temperature_unit;
                entry->temperature_80m.temperature = (f32)temperature;
                entry->temperature_80m.meters_above_ground = 80;
            }
        }

        score_json_array_dispose(&arr);
    }
    */

    score_json_array_dispose(&time_array);

    return SCORE_TRUE;
}

bool open_meteo_create_report_from_json_object(const SCore_JSON_Object *json_object, uint32_t current_flags, Open_Meteo_Hourly_Flags hourly_flags, uint32_t daily_flags, Open_Meteo_Report *out_report) {
    Open_Meteo_Report_Current report_current;
    Open_Meteo_Report_Hourly report_hourly;
    Open_Meteo_Report_Daily report_daily;


    if(json_object == NULL) {
        return false;
    }
    if(out_report == NULL) {
        return false;
    }

    { /* Timezone-abbreviation; "GMT", "GMT+4" etc. */
        SCore_JSON_Object timezone_abbreviation_object;
        SCore_Buffer buf;
        SCore_Buffer_Writer writer;

        if(!score_json_get_object(json_object, "timezone_abbreviation", SCORE_FALSE, &timezone_abbreviation_object)) {
            return false;
        }

        buf = score_buffer_create_from_backing(&out_report->timezone_abbreviation[0], sizeof(out_report->timezone_abbreviation));
        writer = score_buffer_writer_create(&buf);
        if(!score_json_write_to_buffer(&writer, timezone_abbreviation_object)) {
            return false;
        }
    }

    /* Current. */
    printf("Current ...\n");
    {
        memset(&report_current, 0, sizeof(Open_Meteo_Report_Current));
        report_current.flags = current_flags;
    }

    /* Hourly. */
    printf("Hourly ...\n");
    create_hourly_report_from_json_object(json_object, hourly_flags, &report_hourly);

    /* Daily. */
    printf("Daily ...\n");
    {
        memset(&report_daily, 0, sizeof(Open_Meteo_Report_Daily));
        report_daily.flags = daily_flags;
    }

    out_report->current  = report_current;
    out_report->hourly   = report_hourly;
    out_report->daily    = report_daily;

    return true;
}