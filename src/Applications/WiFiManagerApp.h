#ifndef WIFIMANAGER_APP_H
#define WIFIMANAGER_APP_H

#include <Arduino.h>
#include <WiFiManager.h>
#include "datatypes.h"
#include "Filesystem.h"
#include "DisplayFunctions/DisplayFunctions.h"

class WiFiManagerApp{

        private:
        DisplayFunctions* displayFunctions;                                 //To access the displayFunctions
        WiFiManager wm;
        const char* program_selection_str =
        "<p>Please select e-paper mode:</p>"
        "<input style='width: auto; margin: 0 10px 10px 10px;' type='radio' id='choice1' name='program_selection' value='1' checked>"
        "<label for='choice1'>Gold API</label><br>"
        "<input style='width: auto; margin: 10px 10px 10px 10px;' type='radio' id='choice2' name='program_selection' value='2'>"
        "<label for='choice2'>Weather API</label><br>"
        "<input style='width: auto; margin: 10px 10px 0 10px;' type='radio' id='choice3' name='program_selection' value='3'>"
        "<label for='choice3'>Haiku</label><br>";
        const char* API_key_str = "<p>https://www.goldapi.io/ API Key</p>";
        const char* cert_thumbprint_str = "<p>https://www.goldapi.io/ Certification Thumbprint</p>";
        WiFiManagerParameter* program_selection;
        WiFiManagerParameter* API_key;
        WiFiManagerParameter* cert_thumbprint;
        WiFiManagerParameter* API_key_inputfield;
        WiFiManagerParameter* cert_thumbprint_inputfield;

        WeatherAppSettings weatherAppSettings;
        GoldAppSettings goldAppSettings;
       

        public:
        WiFiManagerApp(DisplayFunctions* _displayFunctions);
        bool Run();

        private:
        void AddParameters();
        void saveParamCallback();
        void LoadAppSettings();

};
#endif