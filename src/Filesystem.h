#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <LittleFS.h>
#include "datatypes.h"

static bool FSReady = false;

void initFS();
void StoreSettings(GoldAppSettings settings);
void StoreSettings(WeatherAppSettings settings);
void StoreSettings(WiFiSecrets* settings);
void StoreSettings(ProgramSettings settings);
GoldAppSettings RecoverGoldAppSettings();
WeatherAppSettings RecoverWeatherAppSettings();
WiFiSecrets RecoverWiFiSecrets();
ProgramSettings RecoverProgramSettings();
void deleteAllFiles();

#endif