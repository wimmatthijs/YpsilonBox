#include "LogosApp.h"


LogosApp::LogosApp(DisplayFunctions* _displayFunctions){
    displayFunctions = _displayFunctions;
}

void LogosApp::Run(){
    uint counter = millis();
    displayFunctions->DisplaySecretSanta();
    while(millis()<counter+3333){yield();}
    counter = millis();
    displayFunctions->DisplayWemHackLogo();
    while(millis()<counter+3333){yield();}
    displayFunctions->DisplayYpsilonLogo();
}
