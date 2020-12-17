#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>

struct WeatherAppSettings{
  char* Latitude; //saving as char, will only be used to form strings
  char* Longitude;
  char* Country;
  char* Language;
  char* server = "api.openweathermap.org";
  char* weather_api_key;
};

typedef struct {
  String   Icon;
  String   Forecast0;
  String   Forecast1;
  String   Forecast2;
  float    Temperature;
  float    Humidity;
  float    High;
  float    Low;
  float    Winddir;
  float    Windspeed;
  int      Timezone;
} CurrentWeatherData;

typedef struct {
  int      Dt;
  String   Icon;
  float    Temperature;
  float    High;
  float    Low;
  float    Rainfall;
  int      Sunrise;
  int      Sunset;
} ForecastWeatherData;

struct GoldAppSettings{
  uint8_t fingerprint[20];
  String gold_api_key;
}; 
typedef struct { 
  float high_price_to; // 1847.77
  float high_price_g; // 1847.77
} GoldCourse_record_type;

struct WiFiSecrets{
  String SSID;
  String Pass;
}; 

struct ProgramSettings{
  bool metric;
  bool GoldActive;
  long GoldHour; //in minutes from midnight
  bool WeatherActive;
  long WeatherHour;
  bool LogoActive;
  long LogoHour;
};

struct DeepsleepSettings{
  int deepSleepTimer = 0; //keeping track of the time left to sleep if sleep for more than 3hours (hw limitation) is required.
  int sleptFor      = 0;  //keeps track of how many seconds we slept.
};


#endif /* ifndef FORECAST_RECORD_H_ */
