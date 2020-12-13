#include "DisplayFunctions.h"

//#########################################################################################
DisplayFunctions::DisplayFunctions(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>* _displayPointer, bool _metric){
  displayPointer = _displayPointer;
  metric = _metric;
}
//#########################################################################################
void DisplayFunctions::initialiseDisplay() {
  SPI.begin();
  displayPointer->init(0);
  displayPointer->setRotation(3);
  displayPointer->setTextSize(0);
  displayPointer->setFont(&DejaVu_Sans_Bold_11);
  displayPointer->setTextColor(GxEPD_BLACK);
  displayPointer->fillScreen(GxEPD_WHITE);
  displayPointer->setFullWindow();
}

//#########################################################################################
void DisplayFunctions::setWxReadings(Forecast_record_type*  _WxConditions, Forecast_record_type*  _WxForecast){
  WxConditions = _WxConditions;
  WxForecast = _WxForecast;
}
//#########################################################################################
void DisplayFunctions::setTimeStrings(String* _Time_str, String*_Date_str){
  Time_str = _Time_str;
  Date_str = _Date_str;
}
//#########################################################################################
void DisplayFunctions::setGoldCourse(GoldCourse_record_type* _CurrentCourse){
  CurrentCourse = _CurrentCourse;
}
//#########################################################################################
void DisplayFunctions::DisplayWeather() {                                    // 1.54" e-paper display is 200x200 resolution    
  DisplayHeadingSection();                                 // Top line of the display
  DisplayTempHumiSection(0, 12);                   // Current temperature with Max/Min
  DisplayWxPerson(114, 12, WxConditions[0].Icon);          // Weather person depiction of weather
  DisplayMainWeatherSection(0, 112);               // Weather forecast text
  DisplayForecastSection(0, 135);                  // 3hr interval forecast boxes
  displayPointer->display(false);                                 // Full screen update mode
}
//#########################################################################################
void DisplayFunctions::DisplayGoldCourse() {                         // 1.54" e-paper display is 200x200 resolution    
  displayPointer->fillScreen(GxEPD_WHITE);                        // Start from an empty screen
  DisplayHeadingSection();                                        // Top line of the display
  DisplayGoldPrices();
  displayPointer->display(false);                                 // Full screen update mode
}
//#########################################################################################
void DisplayFunctions::DisplayFunctions::DisplayNoData() {                                    // 1.54" e-paper display is 200x200 resolution
  int x = 0;
  int y = 112;
  displayPointer->drawRect(x, y - 4, SCREEN_WIDTH, 28, GxEPD_BLACK);
  String Wx_Description = WxConditions[0].Forecast0;
  Wx_Description += " ## No Weather ##";
  Wx_Description += " ## No Forecast ##";
  displayPointer->setFont(&DejaVu_Sans_Bold_11);
  drawStringMaxWidth(x + 2, y - 2, 27, TitleCase(Wx_Description), LEFT);
}
//#########################################################################################
void DisplayFunctions::DisplayPowerOff(){
    displayPointer->powerOff(); 
}
//#########################################################################################
//############################INTERNAL FUNCTIONS###########################################
//#########################################################################################
void DisplayFunctions::DisplayTempHumiSection(int x, int y) {
  displayPointer->drawRect(x, y, 115, 97, GxEPD_BLACK);
  displayPointer->setFont(&DSEG7_Classic_Bold_21);
  displayPointer->setTextSize(2);
  drawString(x + 20, y + 5, String(WxConditions[0].Temperature, 0) + "'", LEFT);                                   // Show current Temperature
  displayPointer->setTextSize(1);
  drawString(x + 93, y + 30, (metric ? "C" : "F"), LEFT); // Add-in smaller Temperature unit
  displayPointer->setTextSize(2);
  displayPointer->setFont(&DejaVu_Sans_Bold_11);
  drawString(x + 57, y + 59, String(WxConditions[0].High, 0) + "'/" + String(WxConditions[0].Low, 0) + "'", CENTER); // Show forecast high and Low, in the font ' is a °
  displayPointer->setTextSize(1);
  drawString(x + 60,  y + 83, String(WxConditions[0].Humidity, 0) + "% RH", CENTER);                               // Show Humidity
}
//#########################################################################################
void DisplayFunctions::DisplayHeadingSection() {
  drawString(2, 2,*Time_str, LEFT);
  drawString(SCREEN_WIDTH - 2, 0, *Date_str, RIGHT);
  displayPointer->drawLine(0, 12, SCREEN_WIDTH, 12, GxEPD_BLACK);
}
//#########################################################################################
void DisplayFunctions::DisplayMainWeatherSection(int x, int y) {
  displayPointer->drawRect(x, y - 4, SCREEN_WIDTH, 28, GxEPD_BLACK);
  String Wx_Description = WxConditions[0].Forecast0;
  if (WxConditions[0].Forecast1 != "") Wx_Description += ", " + WxConditions[0].Forecast1;
  if (WxConditions[0].Forecast2 != "") Wx_Description += ", " + WxConditions[0].Forecast2;
  displayPointer->setFont(&DejaVu_Sans_Bold_11);
  Wx_Description += "," + WindDegToDirection(WxConditions[0].Winddir) + " wind, " + String(WxConditions[0].Windspeed, 1) + (metric ? "m/s" : "mph");
  drawStringMaxWidth(x + 2, y - 2, 27, TitleCase(Wx_Description), LEFT);
}
//#########################################################################################
void DisplayFunctions::DisplayForecastSection(int x, int y) {
  int offset = 50;
  DisplayForecastWeather(x + offset * 0, y, offset, 0);
  DisplayForecastWeather(x + offset * 1, y, offset, 1);
  DisplayForecastWeather(x + offset * 2, y, offset, 2);
  DisplayForecastWeather(x + offset * 3, y, offset, 3);
}
//#########################################################################################
void DisplayFunctions::DisplayForecastWeather(int x, int y, int offset, int index) {
  displayPointer->drawRect(x, y, offset, 65, GxEPD_BLACK);
  displayPointer->drawLine(x, y + 13, x + offset, y + 13, GxEPD_BLACK);
  DisplayWxIcon(x + offset / 2 + 1, y + 35, WxForecast[index].Icon, SmallIcon);
  drawString(x + offset / 2, y + 3, String(UnixTimeToString(WxForecast[index].Dt + WxConditions[0].Timezone, metric).substring(0,5)), CENTER);
  drawString(x + offset / 2, y + 50, String(WxForecast[index].High, 0) + "/" + String(WxForecast[index].Low, 0), CENTER);
}
//#########################################################################################
void DisplayFunctions::DisplayRain(int x, int y) {
  if (WxForecast[1].Rainfall > 0) drawString(x, y, String(WxForecast[1].Rainfall, 3) + (metric ? "mm" : "in") + " Rain", LEFT); // Only display rainfall if > 0
}
//#########################################################################################
void DisplayFunctions::DisplayWxIcon(int x, int y, String IconName, bool LargeSize) {
  //Serial.println(IconName);
  if      (IconName == "01d" || IconName == "01n") Sunny(x, y,       LargeSize, IconName);
  else if (IconName == "02d" || IconName == "02n") MostlySunny(x, y, LargeSize, IconName);
  else if (IconName == "03d" || IconName == "03n") Cloudy(x, y,      LargeSize, IconName);
  else if (IconName == "04d" || IconName == "04n") MostlySunny(x, y, LargeSize, IconName);
  else if (IconName == "09d" || IconName == "09n") ChanceRain(x, y,  LargeSize, IconName);
  else if (IconName == "10d" || IconName == "10n") Rain(x, y,        LargeSize, IconName);
  else if (IconName == "11d" || IconName == "11n") Tstorms(x, y,     LargeSize, IconName);
  else if (IconName == "13d" || IconName == "13n") Snow(x, y,        LargeSize, IconName);
  else if (IconName == "50d")                      Haze(x, y,        LargeSize, IconName);
  else if (IconName == "50n")                      Fog(x, y,         LargeSize, IconName);
  else                                             Nodata(x, y,      LargeSize);
}
//#########################################################################################
// Symbols are drawn on a relative 10x10grid and 1 scale unit = 1 drawing unit
void DisplayFunctions::addcloud(int x, int y, int scale, int linesize) {
  //Draw cloud outer
  displayPointer->fillCircle(x - scale * 3, y, scale, GxEPD_BLACK);                      // Left most circle
  displayPointer->fillCircle(x + scale * 3, y, scale, GxEPD_BLACK);                      // Right most circle
  displayPointer->fillCircle(x - scale, y - scale, scale * 1.4, GxEPD_BLACK);            // left middle upper circle
  displayPointer->fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, GxEPD_BLACK); // Right middle upper circle
  displayPointer->fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, GxEPD_BLACK); // Upper and lower lines
  //Clear cloud inner
  displayPointer->fillCircle(x - scale * 3, y, scale - linesize, GxEPD_WHITE);           // Clear left most circle
  displayPointer->fillCircle(x + scale * 3, y, scale - linesize, GxEPD_WHITE);           // Clear right most circle
  displayPointer->fillCircle(x - scale, y - scale, scale * 1.4 - linesize, GxEPD_WHITE); // left middle upper circle
  displayPointer->fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, GxEPD_WHITE); // Right middle upper circle
  displayPointer->fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, GxEPD_WHITE); // Upper and lower lines
}
//#########################################################################################
void DisplayFunctions::addrain(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 0; i < 6; i++) {
    displayPointer->drawLine(x - scale * 4 + scale * i * 1.3 + 0, y + scale * 1.9, x - scale * 3.5 + scale * i * 1.3 + 0, y + scale, GxEPD_BLACK);
    if (scale != Small) {
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.3 + 1, y + scale * 1.9, x - scale * 3.5 + scale * i * 1.3 + 1, y + scale, GxEPD_BLACK);
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.3 + 2, y + scale * 1.9, x - scale * 3.5 + scale * i * 1.3 + 2, y + scale, GxEPD_BLACK);
    }
  }
}
//#########################################################################################
void DisplayFunctions::addsnow(int x, int y, int scale) {
  int dxo, dyo, dxi, dyi;
  for (int flakes = 0; flakes < 5; flakes++) {
    for (int i = 0; i < 360; i = i + 45) {
      dxo = 0.5 * scale * cos((i - 90) * 3.14 / 180); dxi = dxo * 0.1;
      dyo = 0.5 * scale * sin((i - 90) * 3.14 / 180); dyi = dyo * 0.1;
      displayPointer->drawLine(dxo + x + 0 + flakes * 1.5 * scale - scale * 3, dyo + y + scale * 2, dxi + x + 0 + flakes * 1.5 * scale - scale * 3, dyi + y + scale * 2, GxEPD_BLACK);
    }
  }
}
//#########################################################################################
void DisplayFunctions::addtstorm(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 0; i < 5; i++) {
    displayPointer->drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, GxEPD_BLACK);
    if (scale != Small) {
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, GxEPD_BLACK);
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, GxEPD_BLACK);
    }
    displayPointer->drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, GxEPD_BLACK);
    if (scale != Small) {
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, GxEPD_BLACK);
      displayPointer->drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, GxEPD_BLACK);
    }
    displayPointer->drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, GxEPD_BLACK);
    if (scale != Small) {
      displayPointer->drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, GxEPD_BLACK);
      displayPointer->drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, GxEPD_BLACK);
    }
  }
}
//#########################################################################################
void DisplayFunctions::addsun(int x, int y, int scale) {
  int linesize = 1;
  int dxo, dyo, dxi, dyi;
  displayPointer->fillCircle(x, y, scale, GxEPD_BLACK);
  displayPointer->fillCircle(x, y, scale - linesize, GxEPD_WHITE);
  for (float i = 0; i < 360; i = i + 45) {
    dxo = 2.2 * scale * cos((i - 90) * 3.14 / 180); dxi = dxo * 0.6;
    dyo = 2.2 * scale * sin((i - 90) * 3.14 / 180); dyi = dyo * 0.6;
    if (i == 0   || i == 180) {
      displayPointer->drawLine(dxo + x - 1, dyo + y, dxi + x - 1, dyi + y, GxEPD_BLACK);
      if (scale != Small) {
        displayPointer->drawLine(dxo + x + 0, dyo + y, dxi + x + 0, dyi + y, GxEPD_BLACK);
        displayPointer->drawLine(dxo + x + 1, dyo + y, dxi + x + 1, dyi + y, GxEPD_BLACK);
      }
    }
    if (i == 90  || i == 270) {
      displayPointer->drawLine(dxo + x, dyo + y - 1, dxi + x, dyi + y - 1, GxEPD_BLACK);
      if (scale != Small) {
        displayPointer->drawLine(dxo + x, dyo + y + 0, dxi + x, dyi + y + 0, GxEPD_BLACK);
        displayPointer->drawLine(dxo + x, dyo + y + 1, dxi + x, dyi + y + 1, GxEPD_BLACK);
      }
    }
    if (i == 45  || i == 135 || i == 225 || i == 315) {
      displayPointer->drawLine(dxo + x - 1, dyo + y, dxi + x - 1, dyi + y, GxEPD_BLACK);
      if (scale != Small) {
        displayPointer->drawLine(dxo + x + 0, dyo + y, dxi + x + 0, dyi + y, GxEPD_BLACK);
        displayPointer->drawLine(dxo + x + 1, dyo + y, dxi + x + 1, dyi + y, GxEPD_BLACK);
      }
    }
  }
}
//#########################################################################################
void DisplayFunctions::addfog(int x, int y, int scale, int linesize) {
  y -= 10;
  linesize = 1;
  for (int i = 0; i < 6; i++) {
    displayPointer->fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, GxEPD_BLACK);
    displayPointer->fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, GxEPD_BLACK);
    displayPointer->fillRect(x - scale * 3, y + scale * 2.7, scale * 6, linesize, GxEPD_BLACK);
  }
}
//#########################################################################################
void DisplayFunctions::MostlyCloudy(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addsun(x - scale * 1.8, y - scale * 1.8 + offset, scale);
  addcloud(x, y + offset, scale, linesize);
}
//#########################################################################################
void DisplayFunctions::MostlySunny(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addsun(x - scale * 1.8, y - scale * 1.8 + offset, scale);
}
//#########################################################################################
void DisplayFunctions::Rain(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  if (LargeSize) {
    scale = Large;
    offset = 12;
  }
  int linesize = 3;
  if (scale == Small) linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addrain(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::Cloudy(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
}
//#########################################################################################
void DisplayFunctions::Sunny(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  scale = scale * 1.5;
  addsun(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::ExpectRain(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addsun(x - scale * 1.8, y - scale * 1.8 + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addrain(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::ChanceRain(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addsun(x - scale * 1.8, y - scale * 1.8 + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addrain(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::Tstorms(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addtstorm(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::Snow(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addsnow(x, y + offset, scale);
}
//#########################################################################################
void DisplayFunctions::Fog(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addcloud(x, y + offset, scale, linesize);
  addfog(x, y + offset, scale, linesize);
}
//#########################################################################################
void DisplayFunctions::Haze(int x, int y, bool LargeSize, String IconName) {
  int scale = Small, offset = 0;
  int linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale);
  addsun(x, y + offset, scale * 1.4);
  addfog(x, y + offset, scale * 1.4, linesize);
}
//#########################################################################################
void DisplayFunctions::addmoon (int x, int y, int scale) {
  displayPointer->fillCircle(x - 20, y - 15, scale, GxEPD_BLACK);
  displayPointer->fillCircle(x - 15, y - 15, scale * 1.6, GxEPD_WHITE);
}
//#########################################################################################
void DisplayFunctions::Nodata(int x, int y, bool LargeSize) {
  int scale = Small, offset = 0;
  if (LargeSize) {
    scale = Large;
    offset = 7;
  }
  if (scale == Large)  displayPointer->setFont(&FreeMonoBold12pt7b); else displayPointer->setFont(&DejaVu_Sans_Bold_11);
  drawString(x - 20, y - 10 + offset, "N/A", LEFT);
}
//#########################################################################################
void DisplayFunctions::drawString(int x, int y, String text, alignment align) {
  int16_t  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  displayPointer->setTextWrap(false);
  displayPointer->getTextBounds(text, x, y, &x1, &y1, &w, &h);
  if (align == RIGHT)  x = x - w;
  if (align == CENTER) x = x - w / 2;
  displayPointer->setCursor(x, y + h);
  displayPointer->print(text);
}
//#########################################################################################
void DisplayFunctions::drawStringMaxWidth(int x, int y, uint text_width, String text, alignment align) {
  int16_t  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  if (text.length() > text_width * 2) text = text.substring(0, text_width * 2); // Truncate if too long for 2 rows of text
  displayPointer->getTextBounds(text, x, y, &x1, &y1, &w, &h);
  if (align == RIGHT)  x = x - w;
  if (align == CENTER) x = x - w / 2;
  displayPointer->setCursor(x, y + h);
  displayPointer->println(text.substring(0, text_width));
  if (text.length() > text_width) {
    displayPointer->setCursor(x, y + h * 2);
    displayPointer->println(text.substring(text_width));
  }
}
//#########################################################################################
void DisplayFunctions::DisplayWxPerson(int x, int y, String IconName) {
  displayPointer->drawRect(x, y, 86, 97, GxEPD_BLACK);
  x = x + 3;
  y = y + 7;
  // NOTE: Using 'drawInvertedBitmap' and not 'drawBitmap' so that images are WYSIWYG, otherwise all images need to be inverted
  if      (IconName == "01d" || IconName == "01n")  displayPointer->drawInvertedBitmap(x, y, uWX_Sunny,       80, 80, GxEPD_BLACK);
  else if (IconName == "02d" || IconName == "02n")  displayPointer->drawInvertedBitmap(x, y, uWX_MostlySunny, 80, 80, GxEPD_BLACK);
  else if (IconName == "03d" || IconName == "03n")  displayPointer->drawInvertedBitmap(x, y, uWX_Cloudy,      80, 80, GxEPD_BLACK);
  else if (IconName == "04d" || IconName == "04n")  displayPointer->drawInvertedBitmap(x, y, uWX_MostlySunny, 80, 80, GxEPD_BLACK);
  else if (IconName == "09d" || IconName == "09n")  displayPointer->drawInvertedBitmap(x, y, uWX_ChanceRain,  80, 80, GxEPD_BLACK);
  else if (IconName == "10d" || IconName == "10n")  displayPointer->drawInvertedBitmap(x, y, uWX_Rain,        80, 80, GxEPD_BLACK);
  else if (IconName == "11d" || IconName == "11n")  displayPointer->drawInvertedBitmap(x, y, uWX_TStorms,     80, 80, GxEPD_BLACK);
  else if (IconName == "13d" || IconName == "13n")  displayPointer->drawInvertedBitmap(x, y, uWX_Snow,        80, 80, GxEPD_BLACK);
  else if (IconName == "50d")                       displayPointer->drawInvertedBitmap(x, y, uWX_Haze,        80, 80, GxEPD_BLACK);
  else if (IconName == "50n")                       displayPointer->drawInvertedBitmap(x, y, uWX_Fog,         80, 80, GxEPD_BLACK);
  else                                              displayPointer->drawInvertedBitmap(x, y, uWX_Nodata,      80, 80, GxEPD_BLACK);
}
//#########################################################################################
void DisplayFunctions::DisplayYpsilonLogo() {
      displayPointer->fillScreen(GxEPD_WHITE);
      displayPointer->drawBitmap(0, 28, ypsilon, 200, 144, GxEPD_BLACK);
      displayPointer->fillRect(0,28+144-1,200,29,GxEPD_WHITE);
      displayPointer->display(false);
}
//#########################################################################################
void DisplayFunctions::DisplayWemHackLogo() {
      displayPointer->fillScreen(GxEPD_WHITE);
      displayPointer->drawBitmap(0, 0, wemhack, 200, 200, GxEPD_BLACK);
      displayPointer->display(false);
}
//#########################################################################################
void DisplayFunctions::DisplaySecretSanta() {
      displayPointer->fillScreen(GxEPD_WHITE);
      displayPointer->drawBitmap(0, 0, santa, 134, 200, GxEPD_BLACK);
      displayPointer->setFont(&FreeMonoBold12pt7b);
      drawString(180, 20, "SECRET", RIGHT);
      drawString(180, 32, "SANTA", RIGHT);
      drawString(180, 44, "2020", RIGHT);
      displayPointer->display(false);
}
//#########################################################################################
void DisplayFunctions::DisplayNoWiFi() {
      displayPointer->fillScreen(GxEPD_WHITE);
      displayPointer->drawBitmap(10, 10, WiFiError, 180, 180, GxEPD_BLACK);
      displayPointer->display(false);
}
//#########################################################################################
void DisplayFunctions::DisplayServerNotFound(String message) {
      displayPointer->clearScreen();
      displayPointer->fillScreen(GxEPD_WHITE);
      displayPointer->drawBitmap(0, 30, ChromeDino, 200, 135, GxEPD_BLACK);
      displayPointer->setFont(&FreeSansBold9pt7b);
      drawString(100, 170, message, CENTER); 
      displayPointer->display(false);
}
//#########################################################################################
void DisplayFunctions::DisplayGoldPrices() {
      //draw the left gold icon Box
      displayPointer->fillRoundRect(10,16,90,90,10,GxEPD_BLACK);
      displayPointer->fillRoundRect(14,20,82,82,10,GxEPD_WHITE);
      //draw the right silver icon Box
      displayPointer->fillRoundRect(100,106,90,90,10,GxEPD_BLACK);
      displayPointer->fillRoundRect(104,110,82,82,10,GxEPD_WHITE);
      //draw the left gold Element text
      displayPointer->setFont(&FreeSansBold24pt7b);
      drawString(53, 45, "Au", CENTER); 
      //draw the left silver Element text
      displayPointer->setTextSize(1);
      drawString(143, 122, "Ag", CENTER);
      //Display element numbers 
      displayPointer->setFont(&FreeSansBold9pt7b);
      drawString(20, 24, "79", LEFT); 
      drawString(110, 114, "47", LEFT);
      //display price
      displayPointer->setFont(&FreeSansBold18pt7b);

      //Calculating prices to display
      int GoldBeforeComma = (int)trunc(CurrentCourse[0].high_price_g);
      int GoldAfterComma = (int)trunc((CurrentCourse[0].high_price_g-GoldBeforeComma)*100);
      String GoldAfterCommaToDisplay="";
      if (GoldAfterComma <10 ){
        GoldAfterCommaToDisplay="0";
      }
      GoldAfterCommaToDisplay = GoldAfterCommaToDisplay + String(GoldAfterComma);

      int SilverBeforeComma = (int)trunc(CurrentCourse[1].high_price_g*100);
      int SilverAfterComma = (int)trunc((CurrentCourse[1].high_price_g*100-SilverBeforeComma)*100);
      String SilverAfterCommaToDisplay="";
      if (SilverAfterComma <10 ){
        SilverAfterCommaToDisplay="0";
      }
      SilverAfterCommaToDisplay = SilverAfterCommaToDisplay + String(SilverAfterComma);


      drawString(120, 35, String(GoldBeforeComma), LEFT); 
      drawString(20, 125, String(SilverBeforeComma), LEFT);
      displayPointer->setFont(&FreeSansBold9pt7b);
      drawString(160, 35, GoldAfterCommaToDisplay, LEFT); 
      drawString(60, 125, SilverAfterCommaToDisplay, LEFT);
      displayPointer->setFont(&FreeSerifItalic9pt7b);
      drawString(145, 65, "gram", CENTER); 
      drawString(45, 155, "100 gram", CENTER);
}
      

