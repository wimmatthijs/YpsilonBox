#ifndef GOLD_APP_H_
#define GOLD_APP_H_

#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "DisplayFunctions/DisplayFunctions.h"
#include "datatypes.h"
#include "common_functions.h"


class GoldApp{
    
    private:
    GoldAppSettings *settings;
    DisplayFunctions* displayFunctions;                                 //To access the displayFunction
    String* Time_str;
    String* Date_str;
    bool metric;
    GoldCourse_record_type  CurrentCourses[1];  

    public:
    GoldApp(GoldAppSettings* _settings,DisplayFunctions* _displayFunctions, String* _Time_str, String* _Date_str, bool metric);
    void Run();

    private :
    void ErrorMessage();

};

#endif