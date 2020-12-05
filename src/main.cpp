#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define TRIGGER_PIN 0

//******************************************************************
//Function Declatarions (for Arduino IDE compatibilty)
//******************************************************************
//Functions related to WiFi Manager
void saveParamCallback(); //If custom parameters are saved on the webserver this function will be called

//******************************************************************
// global variables
//******************************************************************

//Program variables
uint8_t active_program = 1;

//GOLDAPI.io global parameters
uint8_t fingerprint[20] = {0x82, 0xba, 0x9b, 0x98, 0xf5, 0x32, 0x4a, 0x8a, 0xe3, 0xc1, 0xb0, 0x3c, 0x7a, 0xc2, 0xd3, 0x3f, 0xdf, 0xf1, 0xdb, 0x98};
char* gold_api_key = "goldapi-dwpk2uki9n7dtq-io";

//WifiManager to manage connections
WiFiManager wm; 
//custom parameters input through WiFiManager Webserver
const char* program_selection_str = "<p>Please select e-paper mode:</p>"
  "<input style='width: auto; margin: 0 10px 10px 10px;' type='radio' id='choice1' name='program_selection' value='1' checked>"
  "<label for='choice1'>Gold API</label><br>"
  "<input style='width: auto; margin: 10px 10px 10px 10px;' type='radio' id='choice2' name='program_selection' value='2'>"
  "<label for='choice2'>Weather API</label><br>"
  "<input style='width: auto; margin: 10px 10px 0 10px;' type='radio' id='choice3' name='program_selection' value='3'>"
  "<label for='choice3'>Haiku</label><br>";
WiFiManagerParameter program_selection(program_selection_str);
const char* API_key_str = "<p>https://www.goldapi.io/ API Key</p>";
WiFiManagerParameter API_key(API_key_str);
WiFiManagerParameter API_key_inputfield("API_key_inputfield", "API Key GOLD API", "0", 30);
const char* cert_thumbprint_str = "<p>https://www.goldapi.io/ Certification Thumbprint</p>";
WiFiManagerParameter cert_thumbprint(cert_thumbprint_str);
WiFiManagerParameter cert_thumbprint_inputfield("cert_thumbprint_inputfield", "Certificate Thumbprint", "0", 40);

//Careful : checkbox returns Null if not selected and the value of below defined when selected.
//WiFiManagerParameter checkbox("checkbox", "Een checkbox", "Checkbox selected", 18,"placeholder=\"Custom Field Placeholder\" type=\"checkbox\""); // custom html type


void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);
  
  wm.resetSettings(); // wipe settings for testing purposes

  //test custom html input type(checkbox)
  //wm.addParameter(&checkbox); 
  // test custom html(radio)
  wm.addParameter(&program_selection);
  // add a custom input field
  wm.addParameter(&API_key);
  wm.addParameter(&API_key_inputfield);
  wm.addParameter(&cert_thumbprint);
  wm.addParameter(&cert_thumbprint_inputfield);

  wm.setSaveParamsCallback(saveParamCallback);

  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","info","param","close","sep","restart","exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");


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
  
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("YpsillonBox"); // anonymous ap
  // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
  }
}

void checkButton(){
  // check for button press
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideal for production
      delay(3000); // reset delay hold
      if( digitalRead(TRIGGER_PIN) == LOW ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
      
      // start portal w delay
      Serial.println("Starting config portal");
      wm.setConfigPortalTimeout(300);
      if (!wm.startConfigPortal("YpsillonBox")) {
        Serial.println("failed to connect or hit timeout");
        delay(3000);
        ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
      }
    }
  }
}




void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  //Getting the value of the radio buttons from the server
  String programSelection;
  if(wm.server->hasArg("program_selection")) {
    programSelection = wm.server->arg("program_selection");
  }
  Serial.println("Selected program = " + programSelection);
  //Set the active program accordingly :
  if (programSelection = '1'){
    active_program = 1;
  }
  else if (programSelection = '2'){
    active_program = 2;
  }
  else if (programSelection = '3'){
    active_program = 3;
  }

  Serial.print("Submitted API key = ");
  gold_api_key = strdup(API_key_inputfield.getValue());
  Serial.println(gold_api_key);

  Serial.print("Submitted Certificate Thumbprint = ");
  const char* thumprint=cert_thumbprint_inputfield.getValue();
  Serial.println(thumprint);

  //Conversion of string to fingerprint
  Serial.println("Processing thumbprint");
  char hexstring[3] = {'0','0','\0'};
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

void loop() {
  checkButton();
  // put your main code here, to run repeatedly:
}