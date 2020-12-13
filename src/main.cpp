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
extern "C" {
  #include "user_interface.h" //for being able to access the RTC RAM
}
#include <CRC32.h>    //to check if the data saved in RTC RAM is valid or gobbledeegook

//Application Libs
#include "Applications/WeatherApp.h" // the weather application
#include "Applications/GoldApp.h" // the weather application
#include "Applications/WiFiManagerApp.h" //the app to set the WiFi Password and some settings.
#include "datatypes.h" //datatypes for various objects related to the APIs
#include "Filesystem.h" //All interactions with the filesystem, common usage in all apps.


//******************************************************************
//Function Declatarions (for Platform IO compatibilty)
//******************************************************************
//Functions related to WiFi Manager
void ConnectToWifi();
void saveParamCallback(); //If custom parameters are saved on the webserver this function will be called
//Time-keeping functions
void SetupTime();
//Arduino IDE functions;
void setup();
void loop();
//Program Functions
void BeginSleep(int);
void BackupStateToRTCRAM();
void ReadStateFromRTCRAM();
void RunWiFiManager();


//******************************************************************
// global variables
//******************************************************************

//Defines
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
#define BACKUPLENGTH 8  //number of bytes to be saved in RTC memory, excluding 4 byte CRC
#define METRIC true //change to false for backwards system

//Structs that pass on the needed initial settings to the apps.
WiFiSecrets wiFiSecrets;
WeatherAppSettings weatherAppSettings;
GoldAppSettings goldAppSettings;
ProgramSettings programSettings;

//E-paper instance, this driver is for the Heltec 1.54 inch panel with SSD1681 display controller
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=5*/ EPD_CS, /*DC=17*/ EPD_DC, /*RST=16*/ EPD_RST, /*BUSY=4*/ EPD_BUSY)); //2-colour display Heltec B/W
DisplayFunctions displayFunctions(&display, METRIC);

//timekeeping variables
volatile bool rtc_synced = false;   //keeps track if timesync already occured or not.
static time_t now;
String timeString, dateString; // strings to hold time 
int StartTime, CurrentHour = 0, CurrentMin = 0, CurrentSec = 0; //
long SleepDuration = 30; // Sleep time in minutes, aligned to minute boundary, so if 30 will always update at 00 or 30 past the hour
int WakeupTime    = 0;  // Don't wakeup until after 07:00 to save battery power
int SleepTime     = 24; // Sleep after (23+1) 00:00 to save battery power
int deepSleepTimer = 0; //keeping track of the time left to sleep if sleep for more than 3hours (hw limitation) is required.
int sleptFor      = 0;  //keeps track of how many seconds we slept.
int maxSleepWithoutSync = 12*60*60; //Maximum 12 hours without a timesync. If this time gets exceeded a timesync will be forced.       

 void ConnectToWifi(){
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  initFS();
  if (!LittleFS.exists(F("/WiFiSecrets.txt"))){
      RunWiFiManager();  
  }
  wiFiSecrets = RecoverWiFiSecrets();
  WiFi.begin(wiFiSecrets.SSID, wiFiSecrets.Pass);
  uint counter = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    if(millis()>counter+30000){ //If WiFi not connected after 30 seconds, stop waiting for connection
      displayFunctions.initialiseDisplay();
      displayFunctions.DisplayNoWiFi();
      Serial.println("Failed to connect or hit timeout, going into deepsleep and try again in a few hours...");
      BeginSleep(0); //triggers 3 hours of sleep
      }
    }
  } 

void RunWiFiManager(){
    //recover all current settings from the installed App's.
    WiFiManagerApp wiFiManagerApp(&displayFunctions);
    bool returnvalue = wiFiManagerApp.Run();
    if(returnvalue){
      //Wifi configured correctly, restarting the system (to free up RAM for other apps)
      ESP.restart();
    }
    else{
      //WiFi not set up correctly, go to sleep.
      BeginSleep(0);
    }
}


void ResetWifiRoutine(){
    static const int NumberOfSeconds = 3;
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


void NTP_Sync_Callback(){
  rtc_synced=true;
  sleptFor = 0;   //resetting the slept for counter which keeps track of how long we slept without a timesync.
  Serial.println("time has synchronized");
  Serial.print("Current Unix Epoch Time : ");
  Serial.println(time(nullptr));
  char output[30], day_output[30];
  now = time(nullptr);
  const tm* tm = localtime(&now);
  CurrentHour = tm->tm_hour;
  CurrentMin  = tm->tm_min;
  CurrentSec  = tm->tm_sec;
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

void BeginSleep(int seconds){
  //Maximum deepsleep allowed 3 hours in seconds = 3600 *3 = 10800.
  //redundant code, not really necessary
  deepSleepTimer = seconds;
  if(seconds>10800 || seconds == 0) seconds = 10800;
  WiFi.mode(WIFI_OFF);
  SPI.end();
  long SleepTimer = (long)seconds; 
  Serial.println("Entering " + String(SleepTimer) + "-secs of sleep time");
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Starting deep-sleep period...");
  sleptFor+=SleepTimer;
  deepSleepTimer-=SleepTimer;
  BackupStateToRTCRAM();
  ESP.deepSleep((SleepTimer) * 1000000LL);
}

void BackupStateToRTCRAM(){
  uint8_t byteBuffer[BACKUPLENGTH+4];
  //We will insert the global variables that need to be saved here. in the current conscept 112 bytes. (28 buckets of 4); 
  memcpy(&byteBuffer[0], &sleptFor, sizeof(int));
  memcpy(&byteBuffer[4], &deepSleepTimer, sizeof(int));
  size_t numBytes = sizeof(byteBuffer) - 5; // deduct last byte of the byte buffer, the checsum will be stored here
  CRC32 crc;
  for (size_t i = 0; i < numBytes; i++)
  {
    crc.update(byteBuffer[i]);
  }
  uint32_t checksum = crc.finalize();
  memcpy(&byteBuffer[8], &checksum, sizeof(int));
  int buckets = sizeof(byteBuffer)/4;
  if (buckets == 0) buckets = 1;
  int temp;
  for (int i = 0; i<buckets;i++){
    system_rtc_mem_write(100+i, &byteBuffer[i*4], 4);
    Serial.print("written: ");
    memcpy(&temp, &byteBuffer[i*4], sizeof(int));
    Serial.println(temp);
  }
}

void ReadStateFromRTCRAM(){
  uint8_t byteBuffer[BACKUPLENGTH+4];
  int buckets = sizeof(byteBuffer)/4;
  if (buckets == 0) buckets = 1;
  int temp;
  for (int i = 0; i<buckets;i++){
    system_rtc_mem_read(100+i, &byteBuffer[i*4], 4);
    Serial.print("read: ");
    memcpy(&temp, &byteBuffer[i*4], sizeof(int));
    Serial.println(temp);
  }
  size_t numBytes = sizeof(byteBuffer) - 5; // deduct last byte of the byte buffer, the checsum will be stored here
  CRC32 crc;
  for (size_t i = 0; i < numBytes; i++)
  {
    crc.update(byteBuffer[i]);
  }
  uint32_t calculatedChecksum = crc.finalize();
  Serial.print("Checksum calculated by CRCLib: ");
  Serial.println(calculatedChecksum);
  uint32_t readChecksum;
  memcpy(&readChecksum, &byteBuffer[8], sizeof(uint32_t));
  Serial.print("Checsum as found in buffer: ");
  Serial.println(readChecksum);
  if(calculatedChecksum == readChecksum){
    Serial.println("These checksums are the same, amazing! Restoring program state");
    memcpy(&sleptFor, &byteBuffer[0], sizeof(int));
    memcpy(&deepSleepTimer, &byteBuffer[4], sizeof(int));
  }
  if(deepSleepTimer > 0){
    if(sleptFor > maxSleepWithoutSync){
      Serial.println("Wow, Slept so long that i forgot what time it is, time to resync it");
      ConnectToWifi();
      SetupTime();
      //TODO : Recalculate the left time for the sleeptimer here based on the synched time
    }
    Serial.println("deepSleepTimer didn't expire yet, let's go back to sleep for another cycle");
    BeginSleep(deepSleepTimer);
  }
  else deepSleepTimer = 0;
}


void setup() {

  programSettings.WeatherActive = true;
  programSettings.GoldActive = false;

  StartTime = millis();
  Serial.begin(115200);
  Serial.println("\n Starting");
  pinMode(BUTTON_PIN, INPUT);  
  //If reset button remains pushed, reset the WiFi
  if (digitalRead(BUTTON_PIN) == HIGH){
    ResetWifiRoutine();
  }
  displayFunctions.initialiseDisplay();
  ReadStateFromRTCRAM();
  ConnectToWifi();
  SetupTime();
  if (!rtc_synced){
    uint timenow = millis();
    while(!rtc_synced && millis()<timenow+5000){ //wait for 5 seconds more for the time to come in...
      Serial.println("Waiting for timeserver");
      yield();
      delay(500);
    }
    if(!rtc_synced){
      Serial.println("Didn't get an answer from the timeserver, let's go to sleep");
      displayFunctions.DisplayServerNotFound("Time Server?");
      BeginSleep(0);
    }
  }
  if (rtc_synced){
    //TODO :use UNIX timestamps here update these settings every time they are passed.
    if ((CurrentHour >= WakeupTime && CurrentHour <= SleepTime)) {
      Serial.print("now is the time to execute an application, executing app number : ");
      if(programSettings.GoldActive){
            goldAppSettings = RecoverGoldAppSettings();
            Serial.println("GoldApp will be started");
            GoldApp goldApp(&goldAppSettings,&displayFunctions,&timeString,&dateString, METRIC);
            goldApp.Run();
      }
      else if(programSettings.WeatherActive){
            weatherAppSettings = RecoverWeatherAppSettings();
            Serial.println("WeatherApp will be started");
            WeatherApp weatherApp(&weatherAppSettings,&displayFunctions,&timeString,&dateString, METRIC);
            weatherApp.Run();
      }
    }
  }
  //TODO: calculate required deepsleeptime in seconds reffering to a unix epoch time (setting a traget in unix epoch time);
  BeginSleep(3600);
}

void loop() {
  // put your main code here, to run repeatedly, will never be called in our case
}