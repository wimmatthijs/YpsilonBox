#include "common_functions.h"


float mm_to_inches(float value_mm)
{
  return 0.0393701 * value_mm;
}

float hPa_to_inHg(float value_hPa)
{
  return 0.02953 * value_hPa;
}

int JulianDate(int d, int m, int y) {
  int mm, yy, k1, k2, k3, j;
  yy = y - (int)((12 - m) / 10);
  mm = m + 9;
  if (mm >= 12) mm = mm - 12;
  k1 = (int)(365.25 * (yy + 4712));
  k2 = (int)(30.6001 * mm + 0.5);
  k3 = (int)((int)((yy / 100) + 49) * 0.75) - 38;
  // 'j' for dates in Julian calendar:
  j = k1 + k2 + d + 59 + 1;
  if (j > 2299160) j = j - k3; // 'j' is the Julian date at 12h UT (Universal Time) For Gregorian calendar:
  return j;
}

float SumOfPrecip(float DataArray[], int readings) {
  float sum = 0;
  for (int i = 0; i <= readings; i++) {
    sum += DataArray[i];
  }
  return sum;
}

String TitleCase(String text){
  if (text.length() > 0) {
    String temp_text = text.substring(0,1);
    temp_text.toUpperCase();
    return temp_text + text.substring(1); // Title-case the string
  }
  else return text;
}

double NormalizedMoonPhase(int d, int m, int y) {
  int j = JulianDate(d, m, y);
  //Calculate the approximate phase of the moon
  double Phase = (j + 4.867) / 29.53059;
  return (Phase - (int) Phase);
}

//#########################################################################################
String UnixTimeToString(int unix_time, bool metric) {
  // Returns either '21:12  ' or ' 09:12pm' depending on metric mode
  time_t tm = unix_time;
  struct tm *now_tm = localtime(&tm);
  char output[40];
  if (metric) {
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else {
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}

//#########################################################################################
String WindDegToDirection(float winddirection) {
  if (winddirection >= 348.75 || winddirection < 11.25)  return "N";
  if (winddirection >=  11.25 && winddirection < 33.75)  return "NNO";
  if (winddirection >=  33.75 && winddirection < 56.25)  return "NO";
  if (winddirection >=  56.25 && winddirection < 78.75)  return "ONO";
  if (winddirection >=  78.75 && winddirection < 101.25) return "O";
  if (winddirection >= 101.25 && winddirection < 123.75) return "OZO";
  if (winddirection >= 123.75 && winddirection < 146.25) return "ZO";
  if (winddirection >= 146.25 && winddirection < 168.75) return "ZZO";
  if (winddirection >= 168.75 && winddirection < 191.25) return "Z";
  if (winddirection >= 191.25 && winddirection < 213.75) return "ZZW";
  if (winddirection >= 213.75 && winddirection < 236.25) return "ZW";
  if (winddirection >= 236.25 && winddirection < 258.75) return "WZW";
  if (winddirection >= 258.75 && winddirection < 281.25) return "W";
  if (winddirection >= 281.25 && winddirection < 303.75) return "WNW";
  if (winddirection >= 303.75 && winddirection < 326.25) return "NW";
  if (winddirection >= 326.25 && winddirection < 348.75) return "NNW";
  return "?";
}

float troy_ounce_to_g(float price_in_troyounce){
  return price_in_troyounce/31.1035f;
}

// Returns true if x is in range [low..high], else false 
bool inRange(long low, long high, long x) { 
    bool inrange = false;
    if(x>=low && x<=high) inrange = true;
    return inrange; 
} 
