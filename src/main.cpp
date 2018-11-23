#include <Arduino.h>
#include <serviceFunc.h>
#include <timersFunc.h>
#include <wiFiFunc.h>
#include <fsReadWrite.h>
#include <pingFunc.h>
#include <gsmFunc.h>

extern ipMacInspector *ipMacPrt;
bool setupEnd=false;
void setup() {
  blinkDi(500,10);
  initServiceFunc();
  msg(F("Starting"));
  addUser("User_1","61:02:B4:49:D8:7A","38068127555d");
  addUser("User_2","10:20:30:40:50:60","38068127555d" );
  addUser("User_3_ok","60:02:B4:49:D8:7A","380681275553" );
  initFs();// ----START FS----;
  initWiFi();
  initPing();
  initGsm(); //START GSM

msg(F("setup - end!"));
blinkDi(2000,2);
testFunc();
setupEnd=true;

}

void loop() {
  
wiFiLoop();
timerLoop();
if (setupEnd) loopGsm();

}