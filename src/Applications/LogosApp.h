#ifndef LOGOS_APP_H_
#define LOGOS_APP_H_

#include <Arduino.h>
#include "DisplayFunctions/DisplayFunctions.h"

class LogosApp{
    
    private:
    DisplayFunctions* displayFunctions;                                 //To access the displayFunction

    public:
    LogosApp(DisplayFunctions* _displayFunctions);
    void Run();

};

#endif