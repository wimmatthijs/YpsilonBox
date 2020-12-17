//******************************************************************
// includes
//******************************************************************
//framework lib
#include <Arduino.h>
//WiFi Manager Libs
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
//E-paper Libs
#include <GxEPD2_BW.h>         // GxEPD2 from Sketch, Include Library, Manage Libraries, search for GxEDP2
#include <GxEPD2_3C.h>
#include <Adafruit_I2CDevice.h> //However not used, E-paper libraries have some dependencies on this one
//General Libs
#include <ArduinoJson.h> //for casting data
#include <time.h>   //for doing time stuff
#include <TZ.h>      //timezones
#include "common_functions.h"   //Some unit conversions etc.
#include <coredecls.h> //needed to register time was set callback settimeofday_cb TODO: is this needed?
//Application Libs
#include "Applications/WeatherApp.h" // the weather application
#include "Applications/GoldApp.h" // the weather application
#include "Applications/LogosApp.h" // the weather application
#include "Applications/WiFiManagerApp.h" //the app to set the WiFi Password and some settings.
#include "datatypes.h" //datatypes for various objects related to the APIs
#include "Filesystem.h" //All interactions with the filesystem, common usage in all apps.
#include "RTCRAM.h" //For saving settings in special RAM inside RTC peripheral for settings to be retained through a deepsleepcycle

//******************************************************************
//Function Declatarions (for Platform IO compatibilty)
//******************************************************************
//Functions related to WiFi Manager
void ConnectToWifi(); //Does what it says, goes to deepsleep if unsuccessful
void RunWiFiManager(); //runs in case of hard reset (5 seconds) or first boot (no files in filesystem)
void saveParamCallback(); //If custom parameters are saved on the webserver this function will be called
void ResetWifiRoutine(); //
//Time-keeping functions
void SetupTime(); //configs the time and requests time from server (automatic through time library)
void NTP_Sync_Callback(); //gets called if UDP timepacket arrives (asynchronous callback)
//Program Functions
void CalculateDeepsleep();
void BeginSleep(); //start the next deepsleep period
void DeepsleepMGMT();
//Arduino IDE functions;
void setup();
void loop();

//******************************************************************
// Defines
//******************************************************************
//Pin Connections WARNING : CANNOT USE DESCRIPTIONS D0, D1, .... here! use GPIO numbers!!!!
// E-paper
#define EPD_BUSY 4  // to EPD BUSY
#define EPD_CS   15 // to EPD CS
#define EPD_RST  -1 // Heltec E-ink doesn't have RST pin
#define EPD_DC   0  // to EPD DC
#define EPD_SCK  14 // to EPD CLK
#define EPD_MISO 12 // Not connected, MISO pin is not used, keeping definition just in case it is used somewhere
#define EPD_MOSI 13 // to EPD DIN
//reset applicaiton button
#define BUTTON_PIN 5 //button to reset microcontroller or reset WiFiManger(3s push)
//Application defines
#define TIMEZONE TZ_Europe_Brussels //see https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h for other
#define METRIC true //change to false for backwards system

//******************************************************************
// global variables
//******************************************************************
//Structs that pass on the needed initial settings to the apps.
WiFiSecrets wiFiSecrets;
WeatherAppSettings weatherAppSettings;
GoldAppSettings goldAppSettings;
ProgramSettings programSettings;
DeepsleepSettings deepsleepSettings;
//E-paper instance, this driver is for the Heltec 1.54 inch panel with SSD1681 display controller
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=5*/ EPD_CS, /*DC=17*/ EPD_DC, /*RST=16*/ EPD_RST, /*BUSY=4*/ EPD_BUSY)); //2-colour display Heltec B/W
DisplayFunctions displayFunctions(&display, METRIC);
//timekeeping variables
volatile bool rtc_synced = false;   //keeps track if timesync already occured or not.
static time_t now;
String timeString, dateString; // strings to hold time 
int StartTime, CurrentHour = 0, CurrentMin = 0, CurrentSec = 0;
long CurrentMinuteCounter; //
int maxSleepWithoutSync = 12*60*60; //Maximum 12 hours without a timesync. If this time gets exceeded a timesync will be forced.       

//******************************************************************
// Program
//******************************************************************
void ConnectToWifi(){
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  initFS();
  if (!LittleFS.exists(F("/WiFiSecrets.txt"))){
      RunWiFiManager();  
  }
  wiFiSecrets = RecoverWiFiSecrets();
  wiFiSecrets.Pass = "internet4me";
  wiFiSecrets.SSID = "Wimternet";
  WiFi.begin(wiFiSecrets.SSID, wiFiSecrets.Pass);
  uint counter = millis();
  while (WiFi.status() != WL_CONNECTED) {
    //do something usefull while connecting ;)
    //TODO : can there be more useful stuff here instead of just waiting?
    displayFunctions.initialiseDisplay();
    delay(100);
    Serial.print('.');
  }
  if(millis()>counter+10000){ //If WiFi not connected after 10 seconds, stop waiting for connection
      displayFunctions.DisplayNoWiFi();
      Serial.println("Failed to connect or hit timeout, going into deepsleep and try again in a few hours...");
      deepsleepSettings.deepSleepTimer = 3600; //triggers 3 hours of sleep
      BeginSleep(); 
  }
}

void RunWiFiManager(){
  //recover all current settings from the installed App's.
  WiFiManagerApp wiFiManagerApp(&displayFunctions);
  bool returnvalue = wiFiManagerApp.Run();
  if(returnvalue){
    //Wifi configured correctly, restarting the system (to free up RAM for other apps, WiFImanager doesn't clean up nicely)
    ESP.restart();
  }
  else{
    //WiFi not set up correctly, go to sleep for 3 hours;
    deepsleepSettings.deepSleepTimer = 3600;
    BeginSleep();
  }
}

void ResetWifiRoutine(){
    const int NumberOfSeconds = 3;
    Serial.print("Resetting WiFi credentials");
    while(digitalRead(BUTTON_PIN) == HIGH && millis()<StartTime+NumberOfSeconds*1000UL){
        Serial.print('.');
        yield();
        delay(100);
    }
    Serial.println();
    if (millis()>StartTime+NumberOfSeconds*1000UL){
        RunWiFiManager();
    }
}

void SetupTime() {
  if(!rtc_synced){
    configTime(TIMEZONE, "be.pool.ntp.org");
    settimeofday_cb(NTP_Sync_Callback);
  }
  else{
    Serial.println("time was already synced, using RTC");
  }
  return;
}

void WaitforNTPCallbak(){
  uint timenow = millis();
  while(!rtc_synced && millis()<timenow+5000){ //wait for 5 seconds more for the time to come in...
    Serial.println("Waiting for timeserver");
    yield();
    delay(500);
  }
  if(!rtc_synced){
    Serial.println("Didn't get an answer from the timeserver, let's go to sleep");
    displayFunctions.DisplayServerNotFound("Time Server?");
    //Not able to set up time correctly, go to sleep and try again 3 hours;
    deepsleepSettings.deepSleepTimer = 3600;
    BeginSleep();
  }
}

void NTP_Sync_Callback(){
  rtc_synced=true;
  deepsleepSettings.sleptFor = 0;   //resetting the slept for counter which keeps track of how long we slept without a timesync.
  Serial.println("time has synchronized");
  Serial.print("Current Unix Epoch Time : ");
  Serial.println(time(nullptr));
  char output[30], day_output[30];
  now = time(nullptr);
  const tm* tm = localtime(&now);
  CurrentHour = tm->tm_hour;
  CurrentMin  = tm->tm_min;
  CurrentSec  = tm->tm_sec;
  CurrentMinuteCounter = CurrentHour*60 + CurrentMin;

  //SETTING UP SOME DUMMY DATA FOR THE APPS HERE:
  programSettings.WeatherActive = true;
  programSettings.WeatherHour = CurrentMinuteCounter;
  BackupStateToRTCRAM(programSettings);
  deepsleepSettings.deepSleepTimer = 0;
  deepsleepSettings.sleptFor = 0;
  BackupStateToRTCRAM(deepsleepSettings);

  Serial.print("Current minute couter: ");
  Serial.println(CurrentMinuteCounter);
  #ifdef METRIC
    strftime(day_output, 30, "%d-%b-%y", tm);     // Displays: 24-Jun-17
    strftime(output, 30, "%H:%M", tm);            // Creates: '14:05'
  #else 
    strftime(day_output, 30, "%b-%d-%y", tm);     // Creates: Jun-24-17
    strftime(output, 30, "%I:%M%p", tm);          // Creates: '2:05pm'
  #endif
  dateString = day_output;
  timeString = output;
  Serial.println(dateString);
  Serial.println(timeString);
}

void CalculateDeepsleep(){
  //if no programs are selected, just sleep as long as you can.
  if(!programSettings.GoldActive && !programSettings.WeatherActive && !programSettings.LogoActive){
    deepsleepSettings.deepSleepTimer = INT_MAX;
    return;
  }
  //the difference should me more than 5 to be sure the app didn't just launch prematurely.
  long sleeptimeInMintues = 24*60;
  if(programSettings.GoldActive && programSettings.GoldActive - CurrentMinuteCounter > 5){
    if(programSettings.GoldHour > CurrentMinuteCounter) sleeptimeInMintues = programSettings.GoldHour - CurrentMinuteCounter;
  }
  if(programSettings.WeatherActive){
    if(programSettings.WeatherHour - CurrentMinuteCounter > 5 && programSettings.WeatherHour - CurrentMinuteCounter < sleeptimeInMintues) sleeptimeInMintues = programSettings.WeatherHour - CurrentMinuteCounter;
  }
  if(programSettings.LogoActive){
    if(programSettings.LogoHour - CurrentMinuteCounter > 5 && programSettings.LogoHour - CurrentMinuteCounter < sleeptimeInMintues) sleeptimeInMintues = programSettings.LogoHour - CurrentMinuteCounter;
  }
  //if the counter is stil at 24hours probably we passed the last program for today... check which program should start tomorrow and at which hour
  if(sleeptimeInMintues == 24*60){
    sleeptimeInMintues -= CurrentMinuteCounter;
    Serial.print("minutes left until midnight");
    Serial.println(sleeptimeInMintues);
    int minimum=24*60;
    Serial.print("GoldActitve: ");
    Serial.println(programSettings.GoldActive);
    if(programSettings.GoldActive) minimum = programSettings.GoldHour;
    Serial.print("WeatherActitve: ");
    Serial.println(programSettings.WeatherActive);
    if(programSettings.WeatherActive && programSettings.WeatherHour < minimum ) minimum = programSettings.WeatherHour;
    Serial.print("LogoActitve: ");
    Serial.println(programSettings.LogoActive);
    if(programSettings.LogoActive && programSettings.LogoHour < minimum ) minimum = programSettings.LogoHour; 
    Serial.print("adding the minimum of the apps (next application to start tomorrow): ");
    Serial.println(minimum);
    sleeptimeInMintues += minimum;
  }
  deepsleepSettings.deepSleepTimer = sleeptimeInMintues*60;
  Serial.print("going to sleep for ");
  Serial.print(sleeptimeInMintues);
  Serial.println("minutes");
  return;
}

void BeginSleep(){
  int seconds = deepsleepSettings.deepSleepTimer;
  //Maximum deepsleep allowed 3 hours in seconds = 3600 *3 = 10800.
  if(seconds>10800) seconds = 10800;
  //if(seconds>60) seconds = 60;
  //TODO:redundant?
  displayFunctions.DisplayPowerOff();
  WiFi.mode(WIFI_OFF);
  SPI.end();
  //Settings are changed to have the correct settings on next wake up.
  //After next wake-up the deepsleeptimer will need to sleep x "seconds" less than before (this has passed)
  //After next wake-up the MCU will have sleptfor x "seconds" more without a synchronisation of time
  deepsleepSettings.sleptFor+=seconds;
  deepsleepSettings.deepSleepTimer-=seconds;
  BackupStateToRTCRAM(deepsleepSettings);
  Serial.println("Entering " + String(seconds) + "-secs of sleep time");
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Starting deep-sleep period...");
  ESP.deepSleep((seconds) * 1000000LL);
}

void DeepsleepMGMT(){
  deepsleepSettings = ReadDeepsleepStateFromRTCRAM();
  if(deepsleepSettings.deepSleepTimer > 0){
    if(deepsleepSettings.sleptFor > maxSleepWithoutSync){
      Serial.println("Wow, Slept so long that i forgot what time it is, time to resync it");
      programSettings = ReadProgramStateFromRTCRAM();
      ConnectToWifi();
      SetupTime();
      WaitforNTPCallbak();
      CalculateDeepsleep();
    }
    //No need to alter the deepsleepsetting here, it has been set correctly before going to sleep the previous time (see BeginSleep())
    //Nothing to be done, time is still acceptable, just go to sleep.
    BeginSleep();
  }
}

void setup() {

  //SETTING UP SOME DUMMY DATA FOR THE APPS HERE:
  programSettings.GoldActive = false;
  programSettings.WeatherActive = true;
  programSettings.WeatherActive = false;
  BackupStateToRTCRAM(programSettings);
  deepsleepSettings.deepSleepTimer = 0;
  deepsleepSettings.sleptFor = 0;
  BackupStateToRTCRAM(deepsleepSettings);


  StartTime = millis();
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  
  //If reset button remains pushed, reset the WiFi
  if (digitalRead(BUTTON_PIN) == HIGH){
    ResetWifiRoutine();
  }
  Serial.println("Calling DeepsleepMGMT");
  DeepsleepMGMT();
  //No more deepsleep to finish, let's roll the program!!
  programSettings = ReadProgramStateFromRTCRAM();
  Serial.println("Connect to WiFi");
  ConnectToWifi();
  Serial.println("Calling SetupTime");
  SetupTime();
  if (!rtc_synced){
    WaitforNTPCallbak();
  }
  if (rtc_synced){
    //TODO :use UNIX timestamps here update these settings every time they are passed.
    Serial.println("RTC synced, Entering applications");
    Serial.print("GoldHour is set to: ");
    Serial.println(programSettings.GoldHour);   
    if(programSettings.GoldActive && inRange(programSettings.GoldHour-5LL, programSettings.GoldHour+5LL, CurrentMinuteCounter)){
      goldAppSettings = RecoverGoldAppSettings();
      Serial.println("GoldApp will be started");
      GoldApp goldApp(&goldAppSettings,&displayFunctions,&timeString,&dateString, METRIC);
      goldApp.Run();
    }
    Serial.print("WeatherHour is set to: ");
    Serial.println(programSettings.WeatherHour);
    if(programSettings.WeatherActive && inRange(programSettings.WeatherHour-5LL, programSettings.WeatherHour+5LL, CurrentMinuteCounter)){
      weatherAppSettings = RecoverWeatherAppSettings();
      Serial.println("WeatherApp will be started");
      WeatherApp weatherApp(&weatherAppSettings,&displayFunctions,&timeString,&dateString, METRIC);
      weatherApp.Run();
    }
    Serial.print("LogoHour is set to: ");
    Serial.println(programSettings.LogoHour);
    if(programSettings.LogoActive && inRange(programSettings.LogoHour-5LL, programSettings.LogoHour+5LL, CurrentMinuteCounter)){
      Serial.println("LogoApp will be started");
      LogosApp logosApp(&displayFunctions);
      logosApp.Run();
    }
  }
  //TODO: calculate required deepsleeptime in seconds reffering to a unix epoch time (setting a traget in unix epoch time);
  Serial.println("Calling to calculate deepsleep");
  CalculateDeepsleep();
  Serial.println("Calling to go into deepsleep");
  BeginSleep();
}

void loop() {
  //will never be called in our case
}