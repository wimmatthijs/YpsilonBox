#ifndef WEATER_APP_H
#define WEATHER_APP_H

#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "datatypes.h"
#include <ESP8266HTTPClient.h>
#include "DisplayFunctions/DisplayFunctions.h"
#include "common_functions.h"

//private:

class WeatherApp{

    private:
    CurrentWeatherData  WeatherConditions[1];                                //reference to used WeatherConditions
    ForecastWeatherData  WeatherForecast[4];                                  //reference to used WeatherConditions
    WeatherAppSettings* settings;                                  //rererence to object that contains the used settings
    DisplayFunctions* displayFunctions;                                 //To access the displayFunctions
    String* Time_str;
    String* Date_str;
    bool metric;

    public:
    WeatherApp(WeatherAppSettings* _settings,DisplayFunctions* _displayFunctions, String* _Time_str, String* _Date_str, bool metric);
    void Run();

    private:
    bool obtain_weather_data(WiFiClient &client, const String &jsonfilter);
    bool DecodeCurrentWeather(WiFiClient &json);
    bool DecodeForecastWeather(WiFiClient &json);
};
#endif