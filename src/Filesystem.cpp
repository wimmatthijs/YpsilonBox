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
  String toSave = settings.gold_api_key;
  toSave += '\0';
  for(uint i=0; i < toSave.length(); i++){
    Serial.print(toSave[i]);
    Appinfo.write(toSave[i]);
  }
  for (int i=0; i<20; i++){
    Appinfo.write(settings.fingerprint[i]);
  }
  Appinfo.close();
}

void StoreSettings(WeatherAppSettings settings){
  initFS();
  File Appinfo = LittleFS.open(F("/WeatherAPP.txt"), "w");
  unsigned char * data = reinterpret_cast<unsigned char*>(&settings.Latitude); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.Latitude));
  data = reinterpret_cast<unsigned char*>(&settings.Longitude); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.Longitude));
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

void StoreSettings(ProgramSettings settings){
  initFS();
  File Appinfo = LittleFS.open(F("/ProgramSettings.txt"), "w");
  unsigned char * data = reinterpret_cast<unsigned char*>(&settings.metric); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.metric));
  data = reinterpret_cast<unsigned char*>(&settings.GoldActive); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.GoldActive));
  data = reinterpret_cast<unsigned char*>(&settings.GoldHour); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.GoldHour));
  data = reinterpret_cast<unsigned char*>(&settings.WeatherActive); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.WeatherActive));
  data = reinterpret_cast<unsigned char*>(&settings.WeatherHour); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.WeatherHour));
  data = reinterpret_cast<unsigned char*>(&settings.LogoActive); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.LogoActive));
  data = reinterpret_cast<unsigned char*>(&settings.LogoHour); //casting our struct to the right type for saving into the file.
  Appinfo.write(data, sizeof(settings.LogoHour));
  Appinfo.close();
}


GoldAppSettings RecoverGoldAppSettings(){
  initFS();
  GoldAppSettings settings;
  Serial.println("Attempting recovery of settings");
  File Appinfo = LittleFS.open(F("/GoldAPP.txt"), "r");
  char data[30];
  for (uint i =0; i<30; i++){
    data[i] = Appinfo.read();
    if(data[i] == '\0') break;
  }
  Serial.println(data);
  settings.gold_api_key = data;

  for (int i=0;i<20;i++){
    settings.fingerprint[i] = Appinfo.read();
    Serial.print(settings.fingerprint[i]);
  }
  Serial.println();
  
  Appinfo.close();
  return settings;
}


WeatherAppSettings RecoverWeatherAppSettings(){
  initFS();
  WeatherAppSettings settings;
  File Appinfo = LittleFS.open(F("/WeatherAPP.txt"), "r");

  uint8_t data[sizeof(settings.weather_api_key)];
  for (uint i =0; i< sizeof(settings.Latitude); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.Latitude , data , sizeof(settings.Latitude));

  for (uint i =0; i< sizeof(settings.Longitude); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.Longitude , data , sizeof(settings.Longitude));

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
  Serial.println(settings.Longitude);
  Serial.println(settings.Latitude);
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


ProgramSettings RecoverProgramSettings(){
  initFS();
  ProgramSettings settings;
  File Appinfo = LittleFS.open(F("/ProgramSettings.txt"), "r");

  uint8_t data[sizeof(settings.LogoHour)];
  for (uint i =0; i< sizeof(settings.metric); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.metric , data , sizeof(settings.metric));

  for (uint i =0; i< sizeof(settings.GoldActive); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.GoldActive , data , sizeof(settings.GoldActive));
  
  for (uint i =0; i< sizeof(settings.GoldHour); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.GoldHour , data , sizeof(settings.GoldHour));

  for (uint i =0; i< sizeof(settings.WeatherActive); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.WeatherActive , data , sizeof(settings.WeatherActive));

  for (uint i =0; i< sizeof(settings.WeatherHour); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.WeatherHour , data , sizeof(settings.WeatherHour));

    for (uint i =0; i< sizeof(settings.LogoActive); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.LogoActive , data , sizeof(settings.LogoActive));

  for (uint i =0; i< sizeof(settings.LogoHour); i++){
    data[i] = Appinfo.read();
  }
  memcpy (&settings.LogoHour , data , sizeof(settings.LogoHour));


  Appinfo.close();

  Serial.println("Info recovered : ");
  Serial.println(settings.metric);
  Serial.println(settings.GoldActive);
  Serial.println(settings.GoldHour);
  Serial.println(settings.WeatherActive);
  Serial.println(settings.WeatherHour);
  Serial.println(settings.LogoActive);
  Serial.println(settings.LogoHour);

  return settings;
}

