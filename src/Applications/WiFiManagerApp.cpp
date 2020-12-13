#include "WiFiManagerApp.h"

WiFiManagerApp::WiFiManagerApp(DisplayFunctions* _displayFunctions){
  displayFunctions = _displayFunctions;
}

bool WiFiManagerApp::Run(){
    Serial.println("Resetting WiFi config");
    wm.resetSettings();
    displayFunctions->initialiseDisplay();;
    uint timer = millis();
    displayFunctions->DisplaySecretSanta();
    LoadAppSettings();
    while(millis()<timer+5000L){}
    timer=millis();
    displayFunctions->DisplayWemHackLogo();
    AddParameters();
    // wm.setMenu(menu,6);
    std::vector<const char *> menu = {"wifi","info","param","close","sep","restart","exit"};
    wm.setMenu(menu);

    // set dark theme
    wm.setClass("invert");
    while(millis()<timer+5000L){}
    displayFunctions->DisplayYpsilonLogo();
    displayFunctions->DisplayPowerOff(); //Saves power
    //set static ip
    // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
    // wm.setShowStaticFields(true); // force show static ip fields
    // wm.setShowDnsFields(true);    // force show dns field always

    // wm.setConnectTimeout(20); // how long to try to connect for before continuing
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
    res = wm.autoConnect("YpsilonBox"); // anonymous ap
    // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
    

    if(!res) {
        return false;
        Serial.println("Failed to connect or hit timeout, going into deepsleep and try again in a few hours...");
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :), restarting the app"); //If i don't restart there is not enough memory in the heap to run the apps., Wifimanager doesn't clean up...
        WiFiSecrets wiFiSecrets;
        wiFiSecrets.SSID = wm.getWiFiSSID();
        wiFiSecrets.Pass = wm.getWiFiPass();
        StoreSettings(&wiFiSecrets);
        return true;
    }
}

void WiFiManagerApp::AddParameters(){
    //custom parameters input through WiFiManager 
    Serial.println("creating new sections");
    program_selection = new WiFiManagerParameter(program_selection_str);
    API_key = new WiFiManagerParameter(API_key_str);
    cert_thumbprint = new WiFiManagerParameter(cert_thumbprint_str);
    API_key_inputfield = new WiFiManagerParameter("API_key_inputfield", "API Key GOLD API", "0", 30);
    cert_thumbprint_inputfield = new WiFiManagerParameter("cert_thumbprint_inputfield", "Certificate Thumbprint", "0", 40);

    //Careful : checkbox returns Null if not selected and the value of below defined when selected.
    //WiFiManagerParameter checkbox("checkbox", "Een checkbox", "Checkbox selected", 18,"placeholder=\"Custom Field Placeholder\" type=\"checkbox\""); // custom html type
    //test custom html input type(checkbox)
    //wm.addParameter(&checkbox); 
    // test custom html(radio)
    Serial.println("Adding paramaters to wifimanager");
    wm.addParameter(program_selection);
    // add a custom input field
    wm.addParameter(API_key);
    wm.addParameter(API_key_inputfield);
    wm.addParameter(cert_thumbprint);
    wm.addParameter(cert_thumbprint_inputfield);


    Serial.println("Setting Callback Function");
    wm.setSaveParamsCallback(std::bind(&WiFiManagerApp::saveParamCallback, this));

    // custom menu via array or vector
    // 
    // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
    // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
}



void WiFiManagerApp::saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  


  String programSelection;
  if(wm.server->hasArg("program_selection")) {
    programSelection = wm.server->arg("program_selection");
  }
  Serial.println("Selected program = " + programSelection);
  //Set the active program accordingly :
  if (programSelection == "1"){
    //active_program = 1;
  }
  else if (programSelection == "2"){
    //active_program = 2;
  }
  else if (programSelection == "3"){
    //active_program = 3;
  }

  Serial.print("Submitted API key = ");
  goldAppSettings.gold_api_key = strdup(API_key_inputfield->getValue());
  Serial.println(goldAppSettings.gold_api_key);

  Serial.print("Submitted Certificate Thumbprint = ");
  const char* thumprint=cert_thumbprint_inputfield->getValue();
  Serial.println(thumprint);

  //Conversion of string to fingerprint
  Serial.println("Processing thumbprint");
  char hexstring[3] = {'0','0','\0'};
  uint8_t fingerprint[20];
  for (int i=0;i<20;i++){
    int position = i*2;
    hexstring[0]=thumprint[position];
    hexstring[1]=thumprint[position+1];
    hexstring[2] = '\0';
    Serial.print("Read hex number: ");
    Serial.print(hexstring);
    Serial.print(" ");
    fingerprint[i] = (uint8_t)strtol(hexstring, NULL, 16);
  }
  Serial.print('\n');
  Serial.print("attempted conversion to uint8_t: ");
  for (int i=0;i<20;i++){
    Serial.print(fingerprint[i]);
    Serial.print(" ");
  }
  memcpy(goldAppSettings.fingerprint, fingerprint, sizeof(uint8_t)*20);


  //Careful : checkbox returns Null if not selected and the value of above defined when selected.
  /* Currently not using checkbox
  Serial.print("Submitted checkbox = ");
  String checkBoxSelection;
  if(wm.server->hasArg("checkbox")) {
    programSelection = wm.server->arg("checkbox");
  }
  Serial.println(checkBoxSelection);
  */
}

void WiFiManagerApp::LoadAppSettings(){
  //recovering the API settings from the file system depending on the running application.
  initFS();
  if (!LittleFS.exists(F("/GoldAPP.txt"))){
    Serial.println("Gold API Settings not found in filesystem, writing default values.");
    uint8_t fingerprint[20] = {0x82, 0xba, 0x9b, 0x98, 0xf5, 0x32, 0x4a, 0x8a, 0xe3, 0xc1, 0xb0, 0x3c, 0x7a, 0xc2, 0xd3, 0x3f, 0xdf, 0xf1, 0xdb, 0x98};
    memcpy(goldAppSettings.fingerprint, fingerprint, sizeof(uint8_t)*20);
    goldAppSettings.gold_api_key = "goldapi-dwpk2uki9n7dtq-io";
    StoreSettings(goldAppSettings);
  }
  else{
    Serial.println("File with Gold API settings found in filesystem, attempting recovery");
    goldAppSettings = RecoverGoldAppSettings();
  }
  if (!LittleFS.exists(F("/WeatherAPP.txt"))){
    Serial.println("Weather API Settings not found in filesystem, writing default values.");
    weatherAppSettings.weather_api_key  = "9acbbeebe93eb800c59129e05af24db6";// See: https://openweathermap.org/  // It's free to get an API key, but don't take more than 60 readings/minute!
    weatherAppSettings.City             = "GENK";                            // Your home city See: http://bulk.openweathermap.org/sample/
    weatherAppSettings.Country          = "BE";                              // Your _ISO-3166-1_two-letter_country_code country code, on OWM find your nearest city and the country code is displayed
                                                                // https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
    weatherAppSettings.Language         = "NL";                  // NOTE: Only the weather description is translated by OWM
                                                                // Examples: Arabic (AR) Czech (CZ) English (EN) Greek (EL) Persian(Farsi) (FA) Galician (GL) Hungarian (HU) Japanese (JA)
                                                                // Korean (KR) Latvian (LA) Lithuanian (LT) Macedonian (MK) Slovak (SK) Slovenian (SL) Vietnamese (VI)
    StoreSettings(weatherAppSettings);
  }
  else{
    Serial.println("File with Weather API settings found in filesystem, attempting recovery");
    weatherAppSettings = RecoverWeatherAppSettings();
  }
} 