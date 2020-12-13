#include "Filesystem.h"


void initFS(){
  if(!FSReady){
    Serial.print(F("Inizializing FS..."));  
    if (LittleFS.begin()){
      Serial.println(F("done."));
    }else{
      Serial.println(F("fail."));
    }
    FSReady = true;
  }  
}

void StoreSettings(GoldAppSettings settings){
  initFS();
  File Appinfo = LittleFS.open(F("/GoldAPP.txt"), "w");
  unsigned char * data = reinterpret_cast<unsigned char*>(&settings.gold_api_key); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.gold_api_key));
  data = reinterpret_cast<unsigned char*>(settings.fingerprint);
  Appinfo.write(data, sizeof(settings.fingerprint));
  Appinfo.close();
}

void StoreSettings(WeatherAppSettings settings){
  initFS();
  File Appinfo = LittleFS.open(F("/WeatherAPP.txt"), "w");
  unsigned char * data = reinterpret_cast<unsigned char*>(&settings.City); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.City));
  data = reinterpret_cast<unsigned char*>(&settings.Country); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.Country));
  data = reinterpret_cast<unsigned char*>(&settings.Language); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.Language));
  data = reinterpret_cast<unsigned char*>(&settings.server); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.server));
  data = reinterpret_cast<unsigned char*>(&settings.weather_api_key); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.weather_api_key));  
  Appinfo.close();
}

void StoreSettings(WiFiSecrets* settings){
  initFS();
  File Appinfo = LittleFS.open(F("/WiFiSecrets.txt"), "w");
  Serial.println("Saving the following info to Flash:");
  Serial.println(settings[0].SSID);
  Serial.println(settings[0].Pass);
  String toSave = settings[0].SSID + '\0' + settings[0].Pass + '\0';
  Serial.println(toSave);
  Serial.print("Size: ");
  Serial.println(toSave.length());
  for(uint i=0; i < toSave.length(); i++){
    Serial.println(toSave[i]);
    Appinfo.write(toSave[i]);
  }
  Appinfo.close();
}

GoldAppSettings RecoverGoldAppSettings(){
  initFS();
  GoldAppSettings settings;
  File Appinfo = LittleFS.open(F("/GoldAPP.txt"), "r");

  uint8_t data[sizeof(settings.fingerprint)];
  for (uint i =0; i< sizeof(settings.gold_api_key); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.gold_api_key , data , sizeof(settings.gold_api_key));

  for (uint i =0; i< sizeof(settings.fingerprint); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.fingerprint , data , sizeof(settings.fingerprint));

  Serial.println("Info recovered : ");
  Serial.println(settings.gold_api_key);
  for (int i=0;i<20;i++){
    Serial.print(settings.fingerprint[i]);
  }
  Serial.println();
  return settings;
}


WeatherAppSettings RecoverWeatherAppSettings(){
  initFS();
  WeatherAppSettings settings;
  File Appinfo = LittleFS.open(F("/WeatherAPP.txt"), "r");

  uint8_t data[sizeof(settings.weather_api_key)];
  for (uint i =0; i< sizeof(settings.City); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.City , data , sizeof(settings.City));

  for (uint i =0; i< sizeof(settings.Country); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.Country , data , sizeof(settings.Country));
  
  for (uint i =0; i< sizeof(settings.Language); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.Language , data , sizeof(settings.Language));

  for (uint i =0; i< sizeof(settings.server); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.server , data , sizeof(settings.server));

  for (uint i =0; i< sizeof(settings.weather_api_key); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.weather_api_key , data , sizeof(settings.weather_api_key));

  
  Appinfo.close();

  Serial.println("Info recovered : ");
  Serial.println(settings.City);
  Serial.println(settings.Country);
  Serial.println(settings.Language);
  Serial.println(settings.server);
  Serial.println(settings.weather_api_key);

  return settings;
}

WiFiSecrets RecoverWiFiSecrets(){
  initFS();
  WiFiSecrets secrets;
  Serial.println("Attempting recovery of Wifi Settings");

  File Appinfo = LittleFS.open(F("/WiFiSecrets.txt"), "r");
  char data[50];
  for (uint i =0; i<50; i++){
    data[i] = Appinfo.read();
    Serial.println(data[i]);
    if(data[i] == '\0') break;
  }
  Serial.println(data);
  secrets.SSID = data;

  for (uint i =0; i<50; i++){
    data[i] = Appinfo.read();
    Serial.println(data[i]);
    if(data[i] == '\0') break;
  }
  Serial.println(data);
  secrets.Pass = data;
  
  Appinfo.close();
  Serial.println("Secrets recovered : ");
  Serial.println(secrets.SSID);
  Serial.println(secrets.Pass);

  return secrets;
}

