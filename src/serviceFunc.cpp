#include <Arduino.h>
#include <ESP8266WiFi.h>
#define LOG_MAX_LENGTH 2000

void msg(String);
String toLog(String);

void initServiceFunc(void){
     Serial.begin(115200); 
     msg(F("Msg func init-ok"));

};

bool needReboot=false; // якщо 1 то презавантажити по таймеру
//----REBOOT FUNC-----
void rebootDev(){

  toLog(F("device rebooting..."));
  delay(500);
  WiFi.disconnect(true);
  toLog(F("WiFi - disconnect!"));
  delay(500);
  ESP.restart();
}
//====REBOOT FUNC=====


//--------Blinc------------
void blinkDi(unsigned int timeLagMs, unsigned int countBlink=1){// моргаємо діодом
  pinMode(LED_BUILTIN,OUTPUT);
  bool stLed=true;
  countBlink++;
  unsigned int per=timeLagMs/countBlink;
  while (timeLagMs>=per){
   
   stLed=!stLed;
   digitalWrite(LED_BUILTIN,stLed);  
    timeLagMs-=per; 
    delay(per);   
  }
}; 
//=====Blinc==============

 

//-----MSG-----------


void msg(String txt){
   Serial.println(txt);
};
void msgIn(String txt){
    Serial.print(txt);
};

String toLog(String strToLog){
    msg( strToLog);
    static String t="";
    if (needReboot||t.length()>LOG_MAX_LENGTH){
        t+="SAVING TO FILE...["+String(millis())+"]";
        //Save
        t="";
    }
   t+="["; t+=String(millis());
   t+="]: ";
   t+=strToLog;
   t+="\n";

   return t; 
    // add to log NED
}
//======MSG============ 
 
 