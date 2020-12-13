#ifndef API_CONSTANTS_H
#define API_CONSTANTS_H

#include <Arduino.h>

//Defines
//Pin Connections WARNING : CANNOT USE DESCRIPTIONS D0, D1, .... here! use GPIO numbers!!!!
// E-paper
#define EPD_BUSY 0  // to EPD BUSY
#define EPD_CS   15 // to EPD CS
#define EPD_RST  -1 // Heltec E-ink doesn't have RST pin
#define EPD_DC   2  // to EPD DC
#define EPD_SCK  14 // to EPD CLK
#define EPD_MISO 12 // Not connected, MISO pin is not used, keeping definition just in case it is used somewhere
#define EPD_MOSI 13 // to EPD DIN
//reset applicaiton button
#define BUTTON_PIN 14 //button to reset microcontroller or reset WiFiManger(3s push)
//Application defines
#define TIMEZONE TZ_Europe_Brussels //see https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h for other
#define METRIC  //comment out for backwards system


//TODO : Create a struct of the API information to keep track off and easily store in RTC RAM
//GOLDAPI.io global parameters
uint8_t fingerprint[20] = {0x82, 0xba, 0x9b, 0x98, 0xf5, 0x32, 0x4a, 0x8a, 0xe3, 0xc1, 0xb0, 0x3c, 0x7a, 0xc2, 0xd3, 0x3f, 0xdf, 0xf1, 0xdb, 0x98};
String gold_api_key = "goldapi-dwpk2uki9n7dtq-io";

//WeatherAPI information
String weather_api_key  = "9acbbeebe93eb800c59129e05af24db6";// See: https://openweathermap.org/  // It's free to get an API key, but don't take more than 60 readings/minute!
const char* server     = "api.openweathermap.org";
String City             = "GENK";                            // Your home city See: http://bulk.openweathermap.org/sample/
String Country          = "BE";                              // Your _ISO-3166-1_two-letter_country_code country code, on OWM find your nearest city and the country code is displayed
                                                             // https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
String Language         = "NL";                              // NOTE: Only the weather description is translated by OWM
                                                             // Examples: Arabic (AR) Czech (CZ) English (EN) Greek (EL) Persian(Farsi) (FA) Galician (GL) Hungarian (HU) Japanese (JA)
                                                             // Korean (KR) Latvian (LA) Lithuanian (LT) Macedonian (MK) Slovak (SK) Slovenian (SL) Vietnamese (VI)

#endif