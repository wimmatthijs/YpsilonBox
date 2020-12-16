#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_
#include <Arduino.h>

enum alignment {LEFT, RIGHT, CENTER};

float mm_to_inches(float value_mm);
float hPa_to_inHg(float value_hPa);
int JulianDate(int d, int m, int y);
float SumOfPrecip(float DataArray[], int readings);
String TitleCase(String text);
double NormalizedMoonPhase(int d, int m, int y);
String UnixTimeToString(int unix_time, bool metric);
String WindDegToDirection(float winddirection);
float troy_ounce_to_g(float price_in_troyounce);
bool inRange(long low, long high, long x);

#endif /* ifndef COMMON_FUNCTIONS_H_ */
