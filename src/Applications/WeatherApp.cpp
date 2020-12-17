#include "WeatherApp.h"

WeatherApp::WeatherApp(WeatherAppSettings* _settings,DisplayFunctions* _displayFunctions, String* _Time_str, String* _Date_str, bool _metric){
  settings = _settings;
  displayFunctions = _displayFunctions;
  Time_str = _Time_str;
  Date_str = _Date_str;
  metric = _metric;
}

void WeatherApp::Run(){
  bool ReadWeatherSuccess = false;
  bool ReadForecastSuccess = false;
  displayFunctions->initialiseDisplay(); // Give screen time to initialise by getting weather data!
  //dependency injections for displayFunctions
  displayFunctions->setWxReadings(WeatherConditions, WeatherForecast);
  displayFunctions->setTimeStrings(Time_str, Date_str);
  byte Attempts = 1;
  WiFiClient client;   // wifi client object
  while ((ReadWeatherSuccess == false && ReadForecastSuccess == false) && Attempts <= 2) { // Try up-to 2 times for Weather and Forecast data
    if (ReadWeatherSuccess  == false) ReadWeatherSuccess  = obtain_weather_data(client, "current");
    if (ReadForecastSuccess == false) ReadForecastSuccess = obtain_weather_data(client, "daily");
    Attempts++;
  }
  WiFi.mode(WIFI_OFF); // Reduces power consumption
  if (ReadWeatherSuccess && ReadForecastSuccess) { // If received either Weather or Forecast data then proceed, report later if either failed
    displayFunctions->DisplayWeather();
  }
  else {
    displayFunctions->DisplayServerNotFound("Weatherserver?");
  }
  displayFunctions->DisplayPowerOff();
  SPI.end();
}


//#########################################################################################
bool WeatherApp::obtain_weather_data(WiFiClient& client, const String& jsonfilter) {
  String units = "metric";
  if(!metric)
    units = "imperial";
  client.stop(); // close any existing connection before sending a new request
  HTTPClient http;
  String uri = "/data/2.5/onecall?lat=";
  uri.concat(settings->Latitude);
  uri.concat("&lon=");
  uri.concat(settings->Longitude);
  String excludes = "&exclude=current,daily,minutely,hourly,alerts&APPID=";
  excludes.replace(jsonfilter, ""); //throw out of the exclude what was requested
  uri.concat(excludes);
  uri.concat(settings->weather_api_key);
  uri.concat("&mode=json&units=");
  uri.concat(units);
  uri.concat("&lang=");
  uri.concat(settings->Language);  
  //Serial.println("requested string : ");
  //Serial.print(settings->server);
  //Serial.println(uri);
  http.useHTTP10(true);
  http.begin(client, settings->server, 80, uri);
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    //Serial.println("HTTP_OK Received");
    if(jsonfilter == "current"){
      if (!DecodeCurrentWeather(http.getStream())) return false;
    }
    else if(jsonfilter == "daily"){
      if (!DecodeForecastWeather(http.getStream())) return false;
    }
    client.stop();
    http.end();
    return true;
  }
  else
  {
    //Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
    client.stop();
    http.end();
    return false;
  }
  http.end();
  return true;
}

//#########################################################################################
// Problems with stucturing JSON decodes? see here: https://arduinojson.org/assistant/
bool WeatherApp::DecodeCurrentWeather(WiFiClient& json) {
  //Serial.print(F("\nCreating object...and "));
    
  StaticJsonDocument<160> filter;
  filter["timezone"] = true;
  JsonObject filter_current = filter.createNestedObject("current");
  filter_current["temp"] = true;
  filter_current["humidity"] = true;
  filter_current["wind_speed"] = true;
  filter_current["wind_deg"] = true;
  JsonObject filter_current_weather_0 = filter_current["weather"].createNestedObject();
  filter_current_weather_0["description"] = true;
  filter_current_weather_0["icon"] = true;

  DynamicJsonDocument doc(384);
  DeserializationError error = deserializeJson(doc, json, DeserializationOption::Filter(filter));
  if (error) {
    displayFunctions->DisplayServerNotFound("JSON Fail");
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    return false;
  }

  JsonObject current = doc["current"];
  
  WeatherConditions[0].Forecast0   = current["weather"][0]["description"].as<char*>(); //Serial.println("For0: "+String(WeatherConditions[0].Forecast0));
  WeatherConditions[0].Icon        = current["weather"][0]["icon"].as<char*>();        //Serial.println("Icon: "+String(WeatherConditions[0].Icon));
  WeatherConditions[0].Temperature = current["temp"].as<float>();                      //Serial.println("Temp: "+String(WeatherConditions[0].Temperature));
  WeatherConditions[0].Humidity    = current["humidity"].as<float>();                  //Serial.println("Humi: "+String(WeatherConditions[0].Humidity));
  WeatherConditions[0].Windspeed   = current["wind_speed"].as<float>();                //Serial.println("WSpd: "+String(WeatherConditions[0].Windspeed));
  WeatherConditions[0].Winddir     = current["wind_deg"].as<float>();                  //Serial.println("WDir: "+String(WeatherConditions[0].Winddir));
  WeatherConditions[0].Timezone    = doc["timezone"].as<int>();                        //Serial.println("TZon: "+String(WeatherConditions[0].Timezone));

  return true;
}

bool WeatherApp::DecodeForecastWeather(WiFiClient& json){
  //Serial.print(F("\nCreating object...and "));
  StaticJsonDocument<224> filter;

  JsonObject filter_daily_0 = filter["daily"].createNestedObject();
  filter_daily_0["dt"] = true;

  JsonObject filter_daily_0_temp = filter_daily_0.createNestedObject("temp");
  filter_daily_0_temp["min"] = true;
  filter_daily_0_temp["max"] = true;
  filter_daily_0["humidity"] = true;
  filter_daily_0["wind_speed"] = true;
  filter_daily_0["wind_deg"] = true;

  JsonObject filter_daily_0_weather_0 = filter_daily_0["weather"].createNestedObject();
  filter_daily_0_weather_0["description"] = true;
  filter_daily_0_weather_0["icon"] = true;
  filter_daily_0["rain"] = true;

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, json, DeserializationOption::Filter(filter));
  if (error) {
    displayFunctions->DisplayServerNotFound("JSON Fail");
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    return false;
  }

    JsonArray daily = doc["daily"];
    JsonObject daily_0 = daily[0];
    WeatherConditions[0].Low         = daily_0["temp"]["min"].as<float>();           //Serial.println("TLow: "+String(WeatherConditions[0].Low));
    WeatherConditions[0].High        = daily_0["temp"]["max"].as<float>();           //Serial.println("THig: "+String(WeatherConditions[0].High));

    for (int i=1; i<5;i++){
      daily_0 = daily[i];
      int r = i-1;
      WeatherForecast[r].Dt                = daily_0["dt"].as<int>();                     //Serial.println("DTim: "+String(WeatherForecast[r].Dt));
      WeatherForecast[r].Low               = daily_0["temp"]["min"].as<float>();          //Serial.println("TLow: "+String(WeatherForecast[r].Low));
      WeatherForecast[r].High              = daily_0["temp"]["max"].as<float>();          //Serial.println("THig: "+String(WeatherForecast[r].High));
      WeatherForecast[r].Icon              = daily_0["weather"][0]["icon"].as<char*>();   //Serial.println("Icon: "+String(WeatherForecast[r].Icon));
      WeatherForecast[r].Rainfall          = daily_0["rain"].as<float>();                 //Serial.println("Rain: "+String(WeatherForecast[r].Rainfall));
    }

  return true;
}