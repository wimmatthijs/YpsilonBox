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
  while ((ReadWeatherSuccess == false && ReadForecastSuccess == false) && Attempts <= 3) { // Try up-to 3 times for Weather and Forecast data
    if (ReadWeatherSuccess  == false) ReadWeatherSuccess  = obtain_wx_data(client, "weather");
    if (ReadForecastSuccess == false) ReadForecastSuccess = obtain_wx_data(client, "forecast");
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
bool WeatherApp::obtain_wx_data(WiFiClient& client, const String& RequestType) {
  String units = "metric";
  if(!metric)
    units = "imperial";
  client.stop(); // close any existing connection before sending a new request
  HTTPClient http;
  String uri = "/data/2.5/" + RequestType + "?q=" + settings->City + "," + settings->Country + "&APPID=" + settings->weather_api_key + "&mode=json&units=" + units + "&lang=" + settings->Language;
  ////Serial.println("requested string : ");
  ////Serial.print(settings->server);
  ////Serial.println(uri);
  if(RequestType != "weather")
  {
    uri += "&cnt=" + String(4);
  }
  //http.begin(uri,test_root_ca); //HTTPS example connection
  http.begin(client, settings->server, 80, uri);
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    //Serial.println("HTTP_OK Received");
    if (!DecodeWeather(http.getStream(), RequestType)) return false;
    client.stop();
    http.end();
    return true;
  }
  else
  {
    //Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
    displayFunctions->DisplayServerNotFound("Weatherserver?");
    client.stop();
    http.end();
    return false;
  }
  http.end();
  return true;
}

//#########################################################################################
// Problems with stucturing JSON decodes? see here: https://arduinojson.org/assistant/
bool WeatherApp::DecodeWeather(WiFiClient& json, String Type) {
  //Serial.print(F("\nCreating object...and "));
  // allocate the JsonDocument
  DynamicJsonDocument doc(35 * 1024);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds.
  if (error) {
    displayFunctions->DisplayServerNotFound("JSON Fail");
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    return false;
  }
  // convert it to a JsonObject
  JsonObject root = doc.as<JsonObject>();
  //Serial.println(" Decoding " + Type + " data");
  if (Type == "weather") {
    // All //Serial.println statements are for diagnostic purposes and not required, remove if not needed
    WeatherConditions[0].lon         = root["coord"]["lon"].as<float>();              //Serial.println(" Lon: "+String(WeatherConditions[0].lon));
    WeatherConditions[0].lat         = root["coord"]["lat"].as<float>();              //Serial.println(" Lat: "+String(WeatherConditions[0].lat));
    WeatherConditions[0].Main0       = root["weather"][0]["main"].as<char*>();        //Serial.println("Main: "+String(WeatherConditions[0].Main0));
    WeatherConditions[0].Forecast0   = root["weather"][0]["description"].as<char*>(); //Serial.println("For0: "+String(WeatherConditions[0].Forecast0));
    WeatherConditions[0].Forecast1   = root["weather"][1]["description"].as<char*>(); //Serial.println("For1: "+String(WeatherConditions[0].Forecast1));
    WeatherConditions[0].Forecast2   = root["weather"][2]["description"].as<char*>(); //Serial.println("For2: "+String(WeatherConditions[0].Forecast2));
    WeatherConditions[0].Icon        = root["weather"][0]["icon"].as<char*>();        //Serial.println("Icon: "+String(WeatherConditions[0].Icon));
    WeatherConditions[0].Temperature = root["main"]["temp"].as<float>();              //Serial.println("Temp: "+String(WeatherConditions[0].Temperature));
    WeatherConditions[0].Pressure    = root["main"]["pressure"].as<float>();          //Serial.println("Pres: "+String(WeatherConditions[0].Pressure));
    WeatherConditions[0].Humidity    = root["main"]["humidity"].as<float>();          //Serial.println("Humi: "+String(WeatherConditions[0].Humidity));
    WeatherConditions[0].Low         = root["main"]["temp_min"].as<float>();          //Serial.println("TLow: "+String(WeatherConditions[0].Low));
    WeatherConditions[0].High        = root["main"]["temp_max"].as<float>();          //Serial.println("THig: "+String(WeatherConditions[0].High));
    WeatherConditions[0].Windspeed   = root["wind"]["speed"].as<float>();             //Serial.println("WSpd: "+String(WeatherConditions[0].Windspeed));
    WeatherConditions[0].Winddir     = root["wind"]["deg"].as<float>();               //Serial.println("WDir: "+String(WeatherConditions[0].Winddir));
    WeatherConditions[0].Cloudcover  = root["clouds"]["all"].as<int>();               //Serial.println("CCov: "+String(WeatherConditions[0].Cloudcover)); // in % of cloud cover
    WeatherConditions[0].Visibility  = root["visibility"].as<int>();                  //Serial.println("Visi: "+String(WeatherConditions[0].Visibility)); // in metres
    WeatherConditions[0].Rainfall    = root["rain"]["1h"].as<float>();                //Serial.println("Rain: "+String(WeatherConditions[0].Rainfall));
    WeatherConditions[0].Snowfall    = root["snow"]["1h"].as<float>();                //Serial.println("Snow: "+String(WeatherConditions[0].Snowfall));
    WeatherConditions[0].Country     = root["sys"]["country"].as<char*>();            //Serial.println("Ctry: "+String(WeatherConditions[0].Country));
    WeatherConditions[0].Sunrise     = root["sys"]["sunrise"].as<int>();              //Serial.println("SRis: "+String(WeatherConditions[0].Sunrise));
    WeatherConditions[0].Sunset      = root["sys"]["sunset"].as<int>();               //Serial.println("SSet: "+String(WeatherConditions[0].Sunset));
    WeatherConditions[0].Timezone    = root["timezone"].as<int>();                    //Serial.println("TZon: "+String(WeatherConditions[0].Timezone));
    }
  if (Type == "forecast") {
    ////Serial.println(json);
    //Serial.print(F("\nReceiving Forecast period - ")); //------------------------------------------------
    JsonArray list                    = root["list"];
    for (byte r = 0; r < 4; r++) {
      //Serial.println("\nPeriod-" + String(r) + "--------------");
      WeatherForecast[r].Dt                = list[r]["dt"].as<int>();                          //Serial.println("DTim: "+String(WeatherForecast[r].Dt));
      WeatherForecast[r].Temperature       = list[r]["main"]["temp"].as<float>();              //Serial.println("Temp: "+String(WeatherForecast[r].Temperature));
      WeatherForecast[r].Low               = list[r]["main"]["temp_min"].as<float>();          //Serial.println("TLow: "+String(WeatherForecast[r].Low));
      WeatherForecast[r].High              = list[r]["main"]["temp_max"].as<float>();          //Serial.println("THig: "+String(WeatherForecast[r].High));
      WeatherForecast[r].Pressure          = list[r]["main"]["pressure"].as<float>();          //Serial.println("Pres: "+String(WeatherForecast[r].Pressure));
      WeatherForecast[r].Humidity          = list[r]["main"]["humidity"].as<float>();          //Serial.println("Humi: "+String(WeatherForecast[r].Humidity));
      WeatherForecast[r].Forecast0         = list[r]["weather"][0]["main"].as<char*>();        //Serial.println("For0: "+String(WeatherForecast[r].Forecast0));
      WeatherForecast[r].Forecast1         = list[r]["weather"][1]["main"].as<char*>();        //Serial.println("For1: "+String(WeatherForecast[r].Forecast1));
      WeatherForecast[r].Forecast2         = list[r]["weather"][2]["main"].as<char*>();        //Serial.println("For2: "+String(WeatherForecast[r].Forecast2));
      WeatherForecast[r].Icon              = list[r]["weather"][0]["icon"].as<char*>();        //Serial.println("Icon: "+String(WeatherForecast[r].Icon));
      WeatherForecast[r].Description       = list[r]["weather"][0]["description"].as<char*>(); //Serial.println("Desc: "+String(WeatherForecast[r].Description));
      WeatherForecast[r].Cloudcover        = list[r]["clouds"]["all"].as<int>();               //Serial.println("CCov: "+String(WeatherForecast[r].Cloudcover)); // in % of cloud cover
      WeatherForecast[r].Windspeed         = list[r]["wind"]["speed"].as<float>();             //Serial.println("WSpd: "+String(WeatherForecast[r].Windspeed));
      WeatherForecast[r].Winddir           = list[r]["wind"]["deg"].as<float>();               //Serial.println("WDir: "+String(WeatherForecast[r].Winddir));
      WeatherForecast[r].Rainfall          = list[r]["rain"]["3h"].as<float>();                //Serial.println("Rain: "+String(WeatherForecast[r].Rainfall));
      WeatherForecast[r].Snowfall          = list[r]["snow"]["3h"].as<float>();                //Serial.println("Snow: "+String(WeatherForecast[r].Snowfall));
      WeatherForecast[r].Pop               = list[r]["pop"].as<float>();                       //Serial.println("Pop:  "+String(WeatherForecast[r].Pop));
      WeatherForecast[r].Period            = list[r]["dt_txt"].as<char*>();                    //Serial.println("Peri: "+String(WeatherForecast[r].Period));
    }
    //------------------------------------------
    float pressure_trend = WeatherForecast[0].Pressure - WeatherForecast[2].Pressure; // Measure pressure slope between ~now and later
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0; // Remove any small variations less than 0.1
    WeatherConditions[0].Trend = "0";
    if (pressure_trend > 0)  WeatherConditions[0].Trend = "+";
    if (pressure_trend < 0)  WeatherConditions[0].Trend = "-";
    if (pressure_trend == 0) WeatherConditions[0].Trend = "0";
    if(!metric){
      //Serial.println("converting units to imperial");
      WeatherConditions[0].Pressure = hPa_to_inHg(WeatherConditions[0].Pressure);
      WeatherForecast[1].Rainfall   = mm_to_inches(WeatherForecast[1].Rainfall);
      WeatherForecast[1].Snowfall   = mm_to_inches(WeatherForecast[1].Snowfall);
    }
  }
  return true;
}



