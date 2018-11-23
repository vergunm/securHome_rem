
#include <FS.h>
#include <SPI.h>

#include <serviceFunc.h>

#define MAX_FILE_SIZE 20000
#define NN 100  


struct wiFiConf_t {
    unsigned int id;
    unsigned long time;
    char hostName[15] ;
};


  
bool structToFile(String fileName, wiFiConf_t *d, unsigned int sizeArr){ 
        toLog(fileName);
        if (!SPIFFS.exists(fileName)){
           toLog(F(" - file NO exists! Creating... "));}else{
          delay(10); toLog(F("-file is deleting...!"));
          }
              
 File fileStrim = SPIFFS.open(fileName, "w+");
 if (!fileStrim){ toLog(F("faild o open file to write"));return false;}
    fileStrim.write((const uint8_t *)d, sizeof(*d)*sizeArr);
    /* тут *d - говорить що потрібно  
    брати дані на які вказує вказівник
     а не розмір вказівника */
    fileStrim.close();
    String tmsg=F("File: \"");   
    toLog(tmsg+fileName+"\" seved!");
    
return true;
};

bool structFromFile(String fileName, wiFiConf_t *d,unsigned int sizeArr){
        if (!SPIFFS.exists(fileName)){
          msg("NO file exists! ");}
          
 File fileStrim = SPIFFS.open(fileName, "r");
 if (!fileStrim){ msg("faild o open file");return false;}
    fileStrim.read((uint8_t *)d, sizeof(*d)*sizeArr);
    delay(100);
    fileStrim.close();   
return true;
};
//-----------initFS----------------------
void initFs(void){
     if (!SPIFFS.begin()) { msg(F("failed to mount FS")); return;};
   msg(F("open dir / "));
   { 
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      msg(String(fileName.c_str())+"- FILE");
    }
   }
        msg(F("mounted file system"));

};
//=====initFS==============



bool strToFile(String fileName, String str){ 
       File fileStrim; 
        if (!SPIFFS.exists(fileName)){
          toLog(F("NO file exists! Creating... "));}
          else{
            fileStrim=SPIFFS.open(fileName,"r");
            size_t sz=fileStrim.size();
            fileStrim.close();
             if(sz>MAX_FILE_SIZE){
               msgIn(F("file size: "));msg(String(sz));
               if (SPIFFS.remove(fileName)){
                      delay(10);
                      toLog(fileName); 
                      toLog(F("-file is del!"));}
            }
          }
       //

 fileStrim = SPIFFS.open(fileName, "a");
 if (!fileStrim){ toLog(F("faild o open file to write"));return false;}
    fileStrim.println(str);   
    fileStrim.close();
    msgIn(F("File: ")); msgIn(fileName);msg(F("- saved!"));
delay(500);
return true;
};

  //---test---
void testFunc(void){/*
  msg("Try create struct");
 wiFiConf_t *stOb;
 stOb=new wiFiConf_t[NN];
 for (int i =0;i<NN;i++ ){ 
 strcpy(stOb[i].hostName,"TestName");
 stOb[i].id=i;
 stOb[i].time=millis();
 }
   msg(" struct -done! ");
 //delete []stOb; 
   structToFile("/test.txt",stOb,20); 
   blinkDi(2000,5);
   msg("Write to file end");
wiFiConf_t *stObPrt;
 stObPrt=new wiFiConf_t[NN];

structFromFile("/test.txt",stObPrt,20);
msg(String(stObPrt[0].id));
msg(String(stObPrt[0].hostName));
msg(String("time: ")+String(stObPrt[0].time));
msg(String(stObPrt[19].id));
msg(String(stObPrt[19].hostName));
msg(String("time: ")+String(stObPrt[1].time));
*/
}; 