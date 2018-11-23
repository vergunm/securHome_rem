 
#ifndef SERVICES_FUNC_h
#define SERVICES_FUNC_h
#include <Arduino.h>

//bool needReboot;
//------test Functions------------
//void startTestFunc(void (&func)());
//void testFunc();
//-------------------------------
void initServiceFunc(void);
void msg(String txt);
void msgIn(String txt);
String toLog(String strToLog);
void blinkDi(unsigned int timeLagMs, unsigned int countBlink);


void rebootDev();


#endif
