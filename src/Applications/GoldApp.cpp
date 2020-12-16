#include "GoldApp.h"
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date

GoldApp::GoldApp(GoldAppSettings* _settings,DisplayFunctions* _displayFunctions, String* _Time_str, String* _Date_str, bool _metric){
  settings = _settings;
  displayFunctions = _displayFunctions;
  Time_str = _Time_str;
  Date_str = _Date_str;
  metric = _metric;
}

void GoldApp::Run() {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(settings->fingerprint);

    HTTPClient https;
    bool gotGold, gotSilver = false;
    DynamicJsonDocument doc(48);
    StaticJsonDocument<16> filter;
    filter["high_price"] = true;
    String payload;
    
    Serial.print("[HTTPS] begin for GOLD...\n");
    if (https.begin(*client, "https://www.goldapi.io/api/XAU/EUR")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      https.addHeader("x-access-token",settings->gold_api_key);
      https.addHeader("Content-Type","application/json");
      int httpCode = https.GET();


      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = https.getString();

          deserializeJson(doc, payload, DeserializationOption::Filter(filter));

          CurrentCourses[0].high_price_to = doc["high_price"]; // 1847.77
          CurrentCourses[0].high_price_g = troy_ounce_to_g(CurrentCourses[0].high_price_to);

          Serial.print("high_price: Troy Ounce ");        Serial.println(CurrentCourses[0].high_price_to);
          Serial.print("high_price: Gram ");              Serial.println(CurrentCourses[0].high_price_g);
      
          gotGold = true;
        }
      } else
      {
        client->stop();
        https.end();
        ErrorMessage();
      }
      client->stop();
      https.end();
    } else {
      client->stop();
      https.end();
      ErrorMessage();
      return;
    }
    Serial.print("[HTTPS] begin for Silver...\n");
    if (https.begin(*client, "https://www.goldapi.io/api/XAG/EUR")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      https.addHeader("x-access-token",settings->gold_api_key);
      https.addHeader("Content-Type","application/json");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = https.getString();
          deserializeJson(doc, payload, DeserializationOption::Filter(filter));

          CurrentCourses[1].high_price_to = doc["high_price"]; // 1847.77
          CurrentCourses[1].high_price_g = troy_ounce_to_g(CurrentCourses[1].high_price_to);

          Serial.print("high_price: Troy Ounce ");        Serial.println(CurrentCourses[1].high_price_to);
          Serial.print("high_price: Gram ");              Serial.println(CurrentCourses[1].high_price_g);

          gotSilver = true;
          Serial.println("ending https comms");
          client->stop();
          https.end();
        }
      }
      else
      {
        Serial.println("ending https comms");
        client->stop();
        https.end();
        ErrorMessage();
        return;
      }
    }
    else {
      client->stop();
      https.end();
      ErrorMessage();
      return;
    }
    Serial.println("Switching off Wifi for power saving");
    WiFi.mode(WIFI_OFF); // Reduces power consumption
    Serial.println("displaying Gold information");
    if(gotGold && gotSilver){
      displayFunctions->setTimeStrings(Time_str, Date_str);
      displayFunctions->setGoldCourse(CurrentCourses);
      displayFunctions->initialiseDisplay();
      displayFunctions->DisplayGoldCourse();
    }
    displayFunctions->DisplayPowerOff();
    SPI.end();
    Serial.println("All done, exiting program.");
}

void GoldApp::ErrorMessage(){
    WiFi.mode(WIFI_OFF);
    displayFunctions->DisplayServerNotFound("GoldAPI Server?");
    displayFunctions->DisplayPowerOff();
    SPI.end();
    return;
}
