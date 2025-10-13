#include <stdio.h>

#include "score/types/types.h"
#include "score/console/console.h"
#include "score/string/string.h"

#include "score/integrations/open_meteo/open_meteo.h"
#include "score/integrations/locationiq/locationiq.h"

#include "weather/weather.h"
#include "locationiq_access_token.h"

int main() {
	Weather_Retriever weather_retriever = {0};

	printf("Sweather!\n");

	if(!weather_init_retriever(&weather_retriever)) {
		printf("Failed to initialize weather-retriever.\n");
		return -1;
	}

	while(true) {
		printf("Type the name of a city to check the weather in (or q to quit the program): ");

		{ /* Read from stdin. */
			char console_read_buffer[32];

			if(!score_console_read_from_stdin(&console_read_buffer[0], sizeof(console_read_buffer))) {
				break;
			}

			{ /* Check if we should exit the program. */
				if(score_string_compare(&console_read_buffer[0], "q", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
				if(score_string_compare(&console_read_buffer[0], "exit", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
				if(score_string_compare(&console_read_buffer[0], "quit", false) == SCore_String_Compare_Result_Equal) {
					break;
				}
			}

			{
				uint32_t loc_i;
				SCore_LocationIQ_Response locationiq_response = {0};
				if(!score_locationiq_search(LOCATIONIQ_ACCESS_TOKEN, console_read_buffer, &locationiq_response)) {
					continue;
				}

				if(locationiq_response.locations_found == 0) {
					printf("No locations found.\n");
					continue;
				}

				while(true) {
					char console_read_buffer_location_choice[32];

					for(loc_i = 0; loc_i < locationiq_response.locations_found; loc_i++) {
						SCore_LocationIQ_Location *loc = &locationiq_response.locations[loc_i];
						printf("%i: '%s', Longitude: %.2f, Latitude: %.2f.\n", loc_i, loc->name, loc->coordinate.longitude, loc->coordinate.latitude);
					}

					printf("Select a location (%i-%i): ", 0, locationiq_response.locations_found-1);


					if(!score_console_read_from_stdin(&console_read_buffer_location_choice[0], sizeof(console_read_buffer_location_choice))) {
						break;
					}

					{ /* Check if we should exit the program. */
						if(score_string_compare(&console_read_buffer_location_choice[0], "q", false) == SCore_String_Compare_Result_Equal) {
							break;
						}
						if(score_string_compare(&console_read_buffer_location_choice[0], "exit", false) == SCore_String_Compare_Result_Equal) {
							break;
						}
						if(score_string_compare(&console_read_buffer_location_choice[0], "quit", false) == SCore_String_Compare_Result_Equal) {
							break;
						}
					}

					{ /* Check selection. */
						uint32_t y;
						int32_t selected_location_index = -1;
						for(y = 0; y < locationiq_response.locations_found; y++) {
							if((uint32_t)console_read_buffer_location_choice[0] == '0' + y) {
								selected_location_index = y;
								break;
							}
						}

						if(selected_location_index < 0) {
							printf("Please select a location between 0 and %i.\n", locationiq_response.locations_found-1);
							continue;
						}

						{
							SCore_LocationIQ_Location *loc = &locationiq_response.locations[selected_location_index];

							printf("Checking weather in %s ...\n", loc->name);

							{ /* Build a 'Open_Meteo_Data_Request' and send it off. */
								Open_Meteo_Data_Request weather_data_request = {0};

								weather_data_request.coordinate.longitude= loc->coordinate.longitude;
								weather_data_request.coordinate.latitude = loc->coordinate.latitude;
								weather_data_request.past_days = 0;
								weather_data_request.forecast_days = 2;
								weather_data_request.temperature_unit = Open_Meteo_Unit_Celsius;
								weather_data_request.time_format = Open_Meteo_Unit_Time_Format_UNIX;
								weather_data_request.current_flags = 0;
								weather_data_request.daily_flags = 0;
								weather_data_request.hourly_flags.rain = true;
								weather_data_request.hourly_flags.apparent_temperature = true;
								weather_data_request.hourly_flags.wind_speed_10m = true;
								weather_data_request.hourly_flags.wind_direction_10m = true;
								weather_data_request.hourly_flags.temperature_2m = true;
								weather_data_request.hourly_flags.temperature_80m = true;
								weather_data_request.hourly_flags.temperature_120m = true;
								weather_data_request.hourly_flags.temperature_180m = true;

								{
									Weather_Report report;
									if(weather_get_report(&weather_retriever, &weather_data_request, &report)) {
										printf("Got a report!\n");

										printf("## Open_Meteo_Report\n");
										printf("Time-zone abbreviation: %s.\n", report.open_meteo_report.timezone_abbreviation);
										printf("Current:\n");
										{
											Open_Meteo_Report_Current *v = &report.open_meteo_report.current;
											printf("- Flags: %u.\n", v->flags);
										}
										printf("Hourly:\n");
										{
											Open_Meteo_Report_Hourly *v = &report.open_meteo_report.hourly;
											/*printf("- Flags: %u.\n", v->flags);*/
											printf("- Entry-count: %u.\n", v->entry_count);
											{
												uint32_t i;
												for(i = 0; i < v->entry_count; i++) {
													Open_Meteo_Report_Hourly_Entry *entry = &v->entries[i];

													printf("(%02i) %u-%02u-%02u %02u:%02u:%02d.%02d (%u.%02u)\n",
														i,
														entry->time.iso.year,
														entry->time.iso.month,
														entry->time.iso.day,
														entry->time.iso.hour,
														entry->time.iso.minute,
														(uint8_t)entry->time.iso.second,
														(uint8_t)((entry->time.iso.second - (uint8_t)entry->time.iso.second) * 100 + 0.5f),

														entry->time.unix.seconds_since_1970_01_01,
														entry->time.unix.milliseconds
													);

													{ /* Temperatures.*/
														if(entry->temperature_2m.is_set) {
															Open_Meteo_Report_Temperature *temp = &entry->temperature_2m;
															open_meteo_print_temperature(*temp);
														}
														if(entry->temperature_80m.is_set) {
															Open_Meteo_Report_Temperature *temp = &entry->temperature_80m;
															open_meteo_print_temperature(*temp);
														}
														if(entry->temperature_120m.is_set) {
															Open_Meteo_Report_Temperature *temp = &entry->temperature_120m;
															open_meteo_print_temperature(*temp);
														}
														if(entry->temperature_180m.is_set) {
															Open_Meteo_Report_Temperature *temp = &entry->temperature_180m;
															open_meteo_print_temperature(*temp);
														}
														if(entry->apparent_temperature.is_set) {
															Open_Meteo_Report_Temperature *temp = &entry->apparent_temperature;
															open_meteo_print_apparent_temperature(*temp);
														}
													}
													{ /* Rain. */
														if(entry->rain.is_set) {
															Open_Meteo_Report_Rain *v = &entry->rain;
															open_meteo_print_rain(*v);
														}
													}
													{ /* Wind-speed. */
														if(entry->wind_speed_10m.is_set) {
															Open_Meteo_Report_Wind_Speed *v = &entry->wind_speed_10m;
															open_meteo_print_wind_speed(*v);
														}
														if(entry->wind_speed_80m.is_set) {
															Open_Meteo_Report_Wind_Speed *v = &entry->wind_speed_80m;
															open_meteo_print_wind_speed(*v);
														}
														if(entry->wind_speed_120m.is_set) {
															Open_Meteo_Report_Wind_Speed *v = &entry->wind_speed_120m;
															open_meteo_print_wind_speed(*v);
														}
														if(entry->wind_speed_180m.is_set) {
															Open_Meteo_Report_Wind_Speed *v = &entry->wind_speed_180m;
															open_meteo_print_wind_speed(*v);
														}
													}
													{ /* Wind-direction. */
														if(entry->wind_direction_10m.is_set) {
															Open_Meteo_Report_Wind_Direction *v = &entry->wind_direction_10m;
															open_meteo_print_wind_direction(*v);
														}
														if(entry->wind_direction_80m.is_set) {
															Open_Meteo_Report_Wind_Direction *v = &entry->wind_direction_80m;
															open_meteo_print_wind_direction(*v);
														}
														if(entry->wind_direction_120m.is_set) {
															Open_Meteo_Report_Wind_Direction *v = &entry->wind_direction_120m;
															open_meteo_print_wind_direction(*v);
														}
														if(entry->wind_direction_180m.is_set) {
															Open_Meteo_Report_Wind_Direction *v = &entry->wind_direction_180m;
															open_meteo_print_wind_direction(*v);
														}
													}

													printf("\n");
												}
											}
										}
										printf("Daily:\n");
										{
											Open_Meteo_Report_Daily *v = &report.open_meteo_report.daily;
											printf("- Flags: %u.\n", v->flags);
											printf("- Entry-count: %u.\n", v->entry_count);
										}

										assert(open_meteo_dispose_report(&report.open_meteo_report));
									}
									else {
										printf("Failed to get a report!\n");
									}
								}
							}

							break;
						}
					}
				}
			}
		}


	}

	printf("Program shutting down ...\n");

	weather_dispose_retriever(&weather_retriever);

	return 0;
}