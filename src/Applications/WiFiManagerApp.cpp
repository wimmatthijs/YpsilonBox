#include "WiFiManagerApp.h"

WiFiManagerApp::WiFiManagerApp(DisplayFunctions* _displayFunctions){
  displayFunctions = _displayFunctions;
  wm.setDebugOutput(false);
}

bool WiFiManagerApp::Run(){
    //Serial.println("Resetting WiFi config");
    wm.resetSettings();
    //Serial.println("Resetting Deepsleep Settings");
    //Reset All DeepsleepSettings to start freshly after this hard reset
    DeepsleepSettings deepsleepSettings;
    deepsleepSettings.sleptFor = 0;
    deepsleepSettings.deepSleepTimer = 0;
    BackupStateToRTCRAM(deepsleepSettings);
    displayFunctions->initialiseDisplay();;
    uint timer = millis();
    displayFunctions->DisplaySecretSanta();
    LoadAppSettings();
    while(millis()<timer+5000L){yield();}
    timer=millis();
    displayFunctions->DisplayWemHackLogo();
    AddParameters();
    // wm.setMenu(menu,6);
    //Serial.println("setting the menu vector");
    std::vector<const char *> menu = {"wifi","info","param","exit"};
    wm.setMenu(menu);
    //Serial.println("inverting");
    // set dark theme
    wm.setClass("invert");
    while(millis()<timer+5000L){yield();}
    //Serial.println("displaying last logo");
    displayFunctions->DisplayYpsilonLogo();
    displayFunctions->DisplayPowerOff(); //Saves power
    //set static ip
    // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
    // wm.setShowStaticFields(true); // force show static ip fields
    // wm.setShowDnsFields(true);    // force show dns field always

    // wm.setConnectTimeout(20); // how long to try to connect for before continuing
    //Serial.println("setting config Portal Timeout");
    wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
    // wm.setCaptivePortalEnable(false); // disable captive portal redirection
    // wm.setAPClientCheck(true); // avoid timeout if client connected to softap

    // wifi scan settings
    // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
    // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
    // wm.setShowInfoErase(false);      // do not show erase button on info page
    // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
    
    // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fail
    bool res;
    
    // res = wm.autoConnect(); // auto generated AP name from chipid
    //Serial.println("Calling autoConnect");
    res = wm.autoConnect("YpsilonBox"); // anonymous ap
    // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
    

    if(!res) {
        return false;
        //Serial.println("Failed to connect or hit timeout, going into deepsleep and try again in a few hours...");
    } 
    else {
        //if you get here you have connected to the WiFi    
        //Serial.println("connected...yeey :), restarting the app"); //If i don't restart there is not enough memory in the heap to run the apps., Wifimanager doesn't clean up...
        WiFiSecrets wiFiSecrets;
        wiFiSecrets.SSID = wm.getWiFiSSID();
        wiFiSecrets.Pass = wm.getWiFiPass();
        StoreSettings(&wiFiSecrets);
        return true;
    }
}

void WiFiManagerApp::AddParameters(){
    //custom parameters input through WiFiManager 
    //Serial.println("creating new sections");
    goldAPI_selection = new WiFiManagerParameter(checkboxGoldAPI_str);
    goldAPI_hours = new WiFiManagerParameter(goldAPI_hours_str);
    goldAPI_minutes = new WiFiManagerParameter(goldAPI_minutes_str);
    weatherAPI_selection = new WiFiManagerParameter(checkboxWeatherAPI_str);
    weatherAPI_hours = new WiFiManagerParameter(weatherAPI_hours_str);
    weatherAPI_minutes = new WiFiManagerParameter(weatherAPI_minutes_str);
    logo_selection = new WiFiManagerParameter(checkboxLogo_str);
    logo_hours = new WiFiManagerParameter(logo_hours_str);
    logo_minutes = new WiFiManagerParameter(logo_minutes_str);
    Gold_API_key_inputfield = new WiFiManagerParameter("GOLD_API_key_inputfield", "", "", 30, "placeholder='GoldAPI.IO API key'");
    Gold_cert_thumbprint_inputfield = new WiFiManagerParameter("cert_thumbprint_inputfield", "", "", 40, "placeholder='GoldAPI.IO Certificate Thumbprint'");
    Weather_API_key_inputfield = new WiFiManagerParameter("Weather_API_key_inputfield", "", "", 30, "placeholder='GoldAPI.IO API key'");

    //Serial.println("Adding paramaters to wifimanager");
    wm.addParameter(goldAPI_selection);
    wm.addParameter(goldAPI_hours);
    wm.addParameter(goldAPI_minutes);
    wm.addParameter(weatherAPI_selection);
    wm.addParameter(weatherAPI_hours);
    wm.addParameter(weatherAPI_minutes);
    wm.addParameter(logo_selection);
    wm.addParameter(logo_hours);
    wm.addParameter(logo_minutes);
    wm.addParameter(Gold_API_key_inputfield);
    wm.addParameter(Gold_cert_thumbprint_inputfield);
    wm.addParameter(Weather_API_key_inputfield);
    
    //Serial.println("Setting Callback Function");
    wm.setSaveParamsCallback(std::bind(&WiFiManagerApp::saveParamCallback, this));
    //Serial.println("Callback function set");
}

void WiFiManagerApp::saveParamCallback(){
  //Serial.println("[CALLBACK] saveParamCallback fired");
  //Careful : checkbox returns Null if not selected and the value of above defined when selected.
  //Serial.print("Submitted checkbox = ");
  if(wm.server->hasArg("choice1")) {
    //Serial.println("GOLD API SELECTED");
    programSettings.GoldActive = true;
  }
  else{
    programSettings.GoldActive = false;
  }
  if(wm.server->hasArg("choice2")) {
    //Serial.println("WEATHER API SELECTED");
    programSettings.WeatherActive = true;
  }
  else{
    programSettings.WeatherActive = false;
  }
  if(wm.server->hasArg("choice3")) {
    //Serial.println("LOGO SELECTED");
    programSettings.LogoActive = true;
  }
  else{
    programSettings.LogoActive = false;
  }
  if(wm.server->hasArg("goldAPI_hours")) {
    //Serial.print("goldAPI_hours:");
    programSettings.GoldHour = strtol(wm.server->arg("goldAPI_hours").c_str(),NULL,10)*60;
    //Serial.println(programSettings.GoldHour);
  }
  if(wm.server->hasArg("goldAPI_minutes")) {
    //Serial.print("goldAPI_minutes:");
    programSettings.GoldHour += strtol(wm.server->arg("goldAPI_minutes").c_str(),NULL,10);
    //Serial.println(programSettings.GoldHour);
  }
  if(wm.server->hasArg("weatherAPI_hours")) {
    //Serial.print("weatherAPI_hours:");
    programSettings.WeatherHour = strtol(wm.server->arg("weatherAPI_hours").c_str(),NULL,10)*60;
    //Serial.println(programSettings.WeatherHour);
  }
  if(wm.server->hasArg("weatherAPI_minutes")) {
    //Serial.print("weatherAPI_minutes:");
    programSettings.WeatherHour += strtol(wm.server->arg("weatherAPI_minutes").c_str(),NULL,10);
    //Serial.println(programSettings.WeatherHour);
  }
  if(wm.server->hasArg("logo_hours")) {
    //Serial.print("logo_hours:");
    programSettings.LogoHour = strtol(wm.server->arg("logo_hours").c_str(),NULL,10)*60;
    //Serial.println(programSettings.LogoHour);
  }
  if(wm.server->hasArg("logo_minutes")) {
    //Serial.print("logo_minutes:");
    programSettings.LogoHour += strtol(wm.server->arg("logo_minutes").c_str(),NULL,10);
    //Serial.println(programSettings.LogoHour);
  }
  
  const char* c = Gold_API_key_inputfield->getValue();
  if ((c[0] != '\0')) {
    //Serial.print("Setting Gold API KEY to: ");
    goldAppSettings.gold_api_key = strdup(c);
    //Serial.println(goldAppSettings.gold_api_key);
  }

  c = Gold_cert_thumbprint_inputfield->getValue();
  if ((c[0] != '\0')) {
    //Serial.print("found the following thumbprint: ");
    //Serial.println(c);
    //Conversion of string to fingerprint
    //Serial.println("Processing thumbprint");

    char hexstring[3] = {'0','0','\0'};
    uint8_t fingerprint[20];
    for (int i=0;i<20;i++){
      int position = i*2;
      hexstring[0]=c[position];
      hexstring[1]=c[position+1];
      hexstring[2] = '\0';
      fingerprint[i] = (uint8_t)strtol(hexstring, NULL, 16);
    }
    memcpy(goldAppSettings.fingerprint, fingerprint, sizeof(uint8_t)*20);
    //Serial.print("Set GoldAPI fingerprint to: ");
    for (int i=0;i<20;i++){
      //Serial.print(goldAppSettings.fingerprint[i]);
    }
    //Serial.println();
  }
  
  c = Weather_API_key_inputfield->getValue();
  if ((c[0] != '\0')) {
    //Serial.print("Setting Gold API KEY to: ");
    weatherAppSettings.weather_api_key = strdup(c);
    //Serial.println(weatherAppSettings.weather_api_key);
  }


  //Serial.println("Backing up Program Settings To RTC RAM");
  BackupStateToRTCRAM(programSettings);
  //Serial.println("Backing up Program Settings To Flash");
  //StoreSettings(programSettings);
  //Serial.println("Backing up Weather App Settings To Flash");
  StoreSettings(weatherAppSettings);
  //Serial.println("Backing up Gold App Settings To Flash");
  StoreSettings(goldAppSettings);
  //Serial.println("Backed up all settings, returning");
  return;
}

void WiFiManagerApp::LoadAppSettings(){
  //recovering the API settings from the file system depending on the running application.
  initFS();
  if (!LittleFS.exists(F("/GoldAPP.txt"))){
    //Serial.println("Gold API Settings not found in filesystem, writing default values.");
    uint8_t fingerprint[20] = {0x82, 0xba, 0x9b, 0x98, 0xf5, 0x32, 0x4a, 0x8a, 0xe3, 0xc1, 0xb0, 0x3c, 0x7a, 0xc2, 0xd3, 0x3f, 0xdf, 0xf1, 0xdb, 0x98};
    memcpy(goldAppSettings.fingerprint, fingerprint, sizeof(uint8_t)*20);
    goldAppSettings.gold_api_key = "goldapi-dwpk2uki9n7dtq-io";
    StoreSettings(goldAppSettings);
  }
  else{
    //Serial.println("File with Gold API settings found in filesystem, attempting recovery");
    goldAppSettings = RecoverGoldAppSettings();
  }
  if (!LittleFS.exists(F("/WeatherAPP.txt"))){
    //Serial.println("Weather API Settings not found in filesystem, writing default values.");
    weatherAppSettings.weather_api_key  = "9acbbeebe93eb800c59129e05af24db6";// See: https://openweathermap.org/  // It's free to get an API key, but don't take more than 60 readings/minute!
    weatherAppSettings.Latitude         = "51.04408577099022";
    weatherAppSettings.Longitude        = "5.279147384828176";                            
    weatherAppSettings.Country          = "BE";                     // Your _ISO-3166-1_two-letter_country_code country code, on OWM find your nearest city and the country code is displayed
                                                                // https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
    weatherAppSettings.Language         = "NL";                  // NOTE: Only the weather description is translated by OWM
                                                                // Examples: Arabic (AR) Czech (CZ) English (EN) Greek (EL) Persian(Farsi) (FA) Galician (GL) Hungarian (HU) Japanese (JA)
                                                                // Korean (KR) Latvian (LA) Lithuanian (LT) Macedonian (MK) Slovak (SK) Slovenian (SL) Vietnamese (VI)
    StoreSettings(weatherAppSettings);
  }
  else{
    //Serial.println("File with Weather API settings found in filesystem, attempting recovery");
    weatherAppSettings = RecoverWeatherAppSettings();
  }
  programSettings = ReadProgramStateFromRTCRAM();
} 