#include <Arduino.h>
#include <serviceFunc.h>
#include <IPAddress.h>
#include <pingFunc.h>
extern bool needReboot;

void timerLoop(){
static unsigned long stTime=0; 
static unsigned int t100=0;
 


    if(millis()-stTime>100){//----100ms-----
    stTime=millis();
    t100++;

    //======10ms=======
    }else{
     return;
    };

   if (t100%10==0){
        //----1s----
        if (needReboot){ delay(1000); rebootDev();}// REBOOT if NEED !
         //-------------
            digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN)); //-моргнут діодом раз в 1 сек.
            msgIn(".");
         //------------  
        
        //static byte ip4=1; ip4=ip4<5?(ip4+1):1;
      
        // pingIp(IPAddress(192,168,4,ip4));
       chekUsersOnLine();
        msg(F("----------------------------------"));
        
        
        //====1s====
         //----1min----
        if (t100%600==0){
       
        
        //====1min====
        //--5 min----
        if(t100%600*5==0){
        t100=0;

        }//==== 5 min =====

        };
    };
    


}