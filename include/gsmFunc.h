//gsmFunc.h
#include <Arduino.h>
#ifndef GSM_FUNC_H
#define GSM_FUNC_H

bool initGsm();
bool isHangUpCallNumber(String numb);
void loopGsm( );
String sendAtCommand(String command);
#endif

//gsmFunc.h