#ifndef GOLDAPP_H_
#define GOLDAPP_H_

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
    Gold_APP_information *settings;
    DisplayFunctions* displayFunctions;                                 //To access the displayFunction
    String* Time_str;
    String* Date_str;
    bool metric;
    GoldCourse_record_type  CurrentCourses[1];  

    public:
    GoldApp(Gold_APP_information* _settings,DisplayFunctions* _displayFunctions, String* _Time_str, String* _Date_str, bool metric);
    void Run();

    private :
    void ErrorMessage();

};

#endif