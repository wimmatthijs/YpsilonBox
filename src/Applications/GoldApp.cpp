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
    
    //Serial.print("[HTTPS] begin for GOLD...\n");
    if (https.begin(*client, "https://www.goldapi.io/api/XAU/EUR")) {  // HTTPS

      //Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      https.addHeader("x-access-token","goldapi-dwpk2uki9n7dtq-io");
      https.addHeader("Content-Type","application/json");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            
            DynamicJsonDocument doc(512);
            deserializeJson(doc, payload);

            CurrentCourses[0].timestamp = doc["timestamp"]; // 1607725240                
            CurrentCourses[0].metal = doc["metal"]; // "XAU"
            CurrentCourses[0].currency = doc["currency"]; // "USD"
            CurrentCourses[0].exchange = doc["exchange"]; // "FOREXCOM"
            CurrentCourses[0].symbol = doc["symbol"]; // "FOREXCOM:XAUUSD"
            CurrentCourses[0].prev_close_price_to = doc["prev_close_price"]; // 1836.66
            CurrentCourses[0].open_price_to = doc["open_price"]; // 1836.66
            CurrentCourses[0].low_price_to = doc["low_price"]; // 1824.19
            CurrentCourses[0].high_price_to = doc["high_price"]; // 1847.77
            CurrentCourses[0].open_time = doc["open_time"]; // 1607637600
            CurrentCourses[0].price = doc["price"]; // 1840.13
            CurrentCourses[0].ch = doc["ch"]; // 3.47
            CurrentCourses[0].chp = doc["chp"]; // 0.19
            CurrentCourses[0].ask = doc["ask"]; // 1841.13
            CurrentCourses[0].bid = doc["bid"]; // 1839.13
            //Conversion to gram
            CurrentCourses[0].prev_close_price_g = troy_ounce_to_g(CurrentCourses[0].prev_close_price_to);
            CurrentCourses[0].open_price_g = troy_ounce_to_g(CurrentCourses[0].open_price_to);
            CurrentCourses[0].low_price_g = troy_ounce_to_g(CurrentCourses[0].low_price_to);
            CurrentCourses[0].high_price_g = troy_ounce_to_g(CurrentCourses[0].high_price_to);


            //Serial.print("timestamp: ");                    //Serial.println(CurrentCourses[0].timestamp);
            //Serial.print("metal: ");                        //Serial.println(CurrentCourses[0].metal);
            //Serial.print("currency: ");                     //Serial.println(CurrentCourses[0].currency);
            //Serial.print("exchange: ");                     //Serial.println(CurrentCourses[0].exchange);
            //Serial.print("symbol: ");                       //Serial.println(CurrentCourses[0].symbol);
            //Serial.print("prev_close_price: Troy Ounce ");  //Serial.println(CurrentCourses[0].prev_close_price_to);
            //Serial.print("open_price: Troy Ounce ");        //Serial.println(CurrentCourses[0].open_price_to);
            //Serial.print("low_price: Troy Ounce ");         //Serial.println(CurrentCourses[0].low_price_to);
            //Serial.print("high_price: Troy Ounce ");        //Serial.println(CurrentCourses[0].high_price_to);
            //Serial.print("open_time: ");                    //Serial.println(CurrentCourses[0].open_time);
            //Serial.print("price: ");                        //Serial.println(CurrentCourses[0].price);
            //Serial.print("ch: ");                           //Serial.println(CurrentCourses[0].ch);
            //Serial.print("chp: ");                          //Serial.println(CurrentCourses[0].chp);
            //Serial.print("ask: ");                          //Serial.println(CurrentCourses[0].ask);
            //Serial.print("bid: ");                          //Serial.println(CurrentCourses[0].bid);
            //Serial.print("prev_close_price: Gram ");        //Serial.println(CurrentCourses[0].prev_close_price_g);
            //Serial.print("open_price: Gram ");              //Serial.println(CurrentCourses[0].open_price_g);
            //Serial.print("low_price: Gram ");               //Serial.println(CurrentCourses[0].low_price_g);
            //Serial.print("high_price: Gram ");              //Serial.println(CurrentCourses[0].high_price_g);
        
            gotGold = true;
        }
      } else {
        ErrorMessage();
      }
      https.end();
    } else {
      ErrorMessage();
      return;
    }
    //Serial.print("[HTTPS] begin for Silver...\n");
    if (https.begin(*client, "https://www.goldapi.io/api/XAG/EUR")) {  // HTTPS

      //Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      https.addHeader("x-access-token","goldapi-dwpk2uki9n7dtq-io");
      https.addHeader("Content-Type","application/json");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            
            DynamicJsonDocument doc(512);
            deserializeJson(doc, payload);

            CurrentCourses[1].timestamp = doc["timestamp"]; // 1617725241                
            CurrentCourses[1].metal = doc["metal"]; // "XAU"
            CurrentCourses[1].currency = doc["currency"]; // "USD"
            CurrentCourses[1].exchange = doc["exchange"]; // "FOREXCOM"
            CurrentCourses[1].symbol = doc["symbol"]; // "FOREXCOM:XAUUSD"
            CurrentCourses[1].prev_close_price_to = doc["prev_close_price"]; // 1836.66
            CurrentCourses[1].open_price_to = doc["open_price"]; // 1836.66
            CurrentCourses[1].low_price_to = doc["low_price"]; // 1824.19
            CurrentCourses[1].high_price_to = doc["high_price"]; // 1847.77
            CurrentCourses[1].open_time = doc["open_time"]; // 1607637600
            CurrentCourses[1].price = doc["price"]; // 1840.13
            CurrentCourses[1].ch = doc["ch"]; // 3.47
            CurrentCourses[1].chp = doc["chp"]; // 0.19
            CurrentCourses[1].ask = doc["ask"]; // 1841.13
            CurrentCourses[1].bid = doc["bid"]; // 1839.13
            //Conversion to gram
            CurrentCourses[1].prev_close_price_g = troy_ounce_to_g(CurrentCourses[1].prev_close_price_to);
            CurrentCourses[1].open_price_g = troy_ounce_to_g(CurrentCourses[1].open_price_to);
            CurrentCourses[1].low_price_g = troy_ounce_to_g(CurrentCourses[1].low_price_to);
            CurrentCourses[1].high_price_g = troy_ounce_to_g(CurrentCourses[1].high_price_to);


            //Serial.print("timestamp: ");                    //Serial.println(CurrentCourses[1].timestamp);
            //Serial.print("metal: ");                        //Serial.println(CurrentCourses[1].metal);
            //Serial.print("currency: ");                     //Serial.println(CurrentCourses[1].currency);
            //Serial.print("exchange: ");                     //Serial.println(CurrentCourses[1].exchange);
            //Serial.print("symbol: ");                       //Serial.println(CurrentCourses[1].symbol);
            //Serial.print("prev_close_price: Troy Ounce ");  //Serial.println(CurrentCourses[1].prev_close_price_to);
            //Serial.print("open_price: Troy Ounce ");        //Serial.println(CurrentCourses[1].open_price_to);
            //Serial.print("low_price: Troy Ounce ");         //Serial.println(CurrentCourses[1].low_price_to);
            //Serial.print("high_price: Troy Ounce ");        //Serial.println(CurrentCourses[1].high_price_to);
            //Serial.print("open_time: ");                    //Serial.println(CurrentCourses[1].open_time);
            //Serial.print("price: ");                        //Serial.println(CurrentCourses[1].price);
            //Serial.print("ch: ");                           //Serial.println(CurrentCourses[1].ch);
            //Serial.print("chp: ");                          //Serial.println(CurrentCourses[1].chp);
            //Serial.print("ask: ");                          //Serial.println(CurrentCourses[1].ask);
            //Serial.print("bid: ");                          //Serial.println(CurrentCourses[1].bid);
            //Serial.print("prev_close_price: Gram ");        //Serial.println(CurrentCourses[1].prev_close_price_g);
            //Serial.print("open_price: Gram ");              //Serial.println(CurrentCourses[1].open_price_g);
            //Serial.print("low_price: Gram ");               //Serial.println(CurrentCourses[1].low_price_g);
            //Serial.print("high_price: Gram ");              //Serial.println(CurrentCourses[1].high_price_g);

            gotSilver = true;
        }
      } else {
        ErrorMessage();
        return;
      }
      https.end();
    } else {
      ErrorMessage();
      return;
    }
    WiFi.mode(WIFI_OFF); // Reduces power consumption
    if(gotGold && gotSilver){
      displayFunctions->setTimeStrings(Time_str, Date_str);
      displayFunctions->setGoldCourse(CurrentCourses);
      displayFunctions->initialiseDisplay();
      displayFunctions->DisplayGoldCourse();
    }

    displayFunctions->DisplayPowerOff();
    SPI.end();
}

void GoldApp::ErrorMessage(){
    WiFi.mode(WIFI_OFF);
    displayFunctions->DisplayServerNotFound("GoldAPI Server?");
    displayFunctions->DisplayPowerOff();
    SPI.end();
    return;
}
