#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>

struct Weather_APP_information{
  char* City;
  char* Country;
  char* Language;
  char* server = "api.openweathermap.org";
  char* weather_api_key;
};

typedef struct { // For current Day and Day 1, 2, 3, etc
  int      Dt;
  String   Period;
  String   Icon;
  String   Trend;
  String   Main0;
  String   Forecast0;
  String   Forecast1;
  String   Forecast2;
  String   Description;
  String   Time;
  String   Country;
  float    lat;
  float    lon;
  float    Temperature;
  float    Humidity;
  float    High;
  float    Low;
  float    Winddir;
  float    Windspeed;
  float    Rainfall;
  float    Snowfall;
  float    Pop;
  float    Pressure;
  int      Cloudcover;
  int      Visibility;
  int      Sunrise;
  int      Sunset;
  int      Timezone;
} Forecast_record_type;

struct Gold_APP_information{
  uint8_t fingerprint[20];
  char* gold_api_key;
}; 
typedef struct { // For current Day and Day 1, 2, 3, etc
  long timestamp; // 1607725240                
  const char* metal; // "XAU"
  const char* currency; // "USD"
  const char* exchange; // "FOREXCOM"
  const char* symbol; // "FOREXCOM:XAUUSD"
  float prev_close_price_to; // 1836.66
  float open_price_to; // 1836.66
  float low_price_to; // 1824.19
  float high_price_to; // 1847.77
  float prev_close_price_g; // 1836.66
  float open_price_g; // 1836.66
  float low_price_g; // 1824.19
  float high_price_g; // 1847.77
  long open_time; // 1607637600
  float price; // 1840.13
  float ch; // 3.47
  float chp; // 0.19
  float ask; // 1841.13
  float bid; // 1839.13
} GoldCourse_record_type;

struct WiFiSecrets{
  String SSID;
  String Pass;
}; 



#endif /* ifndef FORECAST_RECORD_H_ */
