#ifndef DISPLAY_FUNCTIONS_H_
#define DISPLAY_FUNCTIONS_H_

#include "common_functions.h"
#include <GxEPD2_BW.h>         
#include <GxEPD2_3C.h>
#include "datatypes.h"
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include "epaper_fonts.h"
#include "Micro_Wx_Icons.h"    // Weather Icons
#include "Logos.h"

//some settings regarding the display
#define ENABLE_GxEPD2_display 0
#define SCREEN_WIDTH  200
#define SCREEN_HEIGHT 200
#define Large  10
#define Small  4
#define LargeIcon true
#define SmallIcon false

class DisplayFunctions {

  private:
  //dependency injections
  CurrentWeatherData*  WxConditions;  //reference to used WeatherConditions
  ForecastWeatherData*  WxForecast;    //reference to used WeatherConditions
  GoldCourse_record_type* CurrentCourse;   //reference to gold cours information
  GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>* displayPointer;  //displayPointer
  String* Time_str;
  String* Date_str;
  bool metric;
  
  public:

  //constructor
  DisplayFunctions(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>* _displayPointer, bool _metric);
  
  //functions for data binding
  void setWxReadings(CurrentWeatherData*  _WxConditions, ForecastWeatherData*  _WxForecast);
  void setTimeStrings(String* _Time_str, String*_Date_Str);
  void setGoldCourse(GoldCourse_record_type* _CurrentCourse);
  
  //functions for displaying weather/golf info onto E-ink Display
  void DisplayWeather();
  void DisplayGoldCourse();
  void DisplayNoData();
  void DisplayYpsilonLogo();
  void DisplayWemHackLogo();
  void DisplaySecretSanta();


  //general
  void initialiseDisplay();
  void DisplayPowerOff();
  void DisplayNoWiFi();
  void DisplayServerNotFound(String message);
  

  //internal
  private:
  void DisplayTempHumiSection(int x, int y);
  void DisplayHeadingSection();
  void DisplayMainWeatherSection(int x, int y);
  void DisplayForecastSection(int x, int y);
  void DisplayForecastWeather(int x, int y, int offset, int index);
  void DisplayRain(int x, int y);                          //no function call to this function?
  void DisplayWxIcon(int x, int y, String IconName, bool LargeSize);
  void addcloud(int x, int y, int scale, int linesize);
  void addrain(int x, int y, int scale);
  void addsnow(int x, int y, int scale);
  void addtstorm(int x, int y, int scale);
  void addsun(int x, int y, int scale);
  void addfog(int x, int y, int scale, int linesize);
  void MostlyCloudy(int x, int y, bool LargeSize, String IconName);
  void MostlySunny(int x, int y, bool LargeSize, String IconName);
  void Rain(int x, int y, bool LargeSize, String IconName);
  void Cloudy(int x, int y, bool LargeSize, String IconName);
  void Sunny(int x, int y, bool LargeSize, String IconName);
  void ExpectRain(int x, int y, bool LargeSize, String IconName);
  void ChanceRain(int x, int y, bool LargeSize, String IconName);
  void Tstorms(int x, int y, bool LargeSize, String IconName);
  void Snow(int x, int y, bool LargeSize, String IconName);
  void Fog(int x, int y, bool LargeSize, String IconName);
  void Haze(int x, int y, bool LargeSize, String IconName);
  void addmoon (int x, int y, int scale);
  void Nodata(int x, int y, bool LargeSize);
  void drawString(int x, int y, String text, alignment align);
  void drawStringMaxWidth(int x, int y, uint text_width, String text, alignment align);
  void DisplayWxPerson(int x, int y, String IconName);
  void DisplayGoldPrices();
};
#endif
