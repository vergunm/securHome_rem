#include <Arduino.h>
#include <serviceFunc.h>
#include <pingFunc.h>

/**************************************************************
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 * NOTE:
 * Some of the functions may be unavailable for your modem.
 * Just comment them out.
 *
 **************************************************************/

/*


*/
#define TINY_GSM_MODEM_A6

#define SerialMon Serial

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(D7, D8); // RX, TX

//#define TINY_GSM_DEBUG SerialMon

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);

void setPortRate(unsigned int rate = 19200)
{

  msg(F("Trying baud rate "));
  SerialAT.begin(rate);
  delay(10);
  for (int i = 0; i < 10; i++)
  {
    SerialAT.print(F("AT\r\n"));

    String input = SerialAT.readString();
    if (input.indexOf("OK") >= 0)
    {
      msgIn(F("Modem responded at rate: "));
      msg(String(rate));
      delay(50);
      SerialAT.print("ATZ+IPR=" + String(rate) + "\r\n");
      toLog(SerialAT.readString());
      delay(50);
      SerialAT.print(F("AT+CLIP=1\r\n"));
      toLog(SerialAT.readString());
      delay(50);
      SerialAT.print(F("AT+DDET=1\r\n"));
      toLog(SerialAT.readString());
      delay(50);
      SerialAT.print(F("AT&W\r\n"));
      toLog(SerialAT.readString());
      return;
    }
  }
  return;
}

void GsmPwrOn(byte pinPwr)
{
  pinMode(pinPwr, OUTPUT);
  digitalWrite(pinPwr, HIGH);
  delay(2000);
  digitalWrite(pinPwr, HIGH);
}

bool initGsm()
{

  // Set your reset, enable, power pins here
  /////////////////////////////
  // GsmPwrOn( pinPwr);
  //////////////////////////
  delay(2000);

  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT);
  setPortRate();

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  msg(F("Initializing modem..."));
  if (!modem.init())
  {
    delay(1000);
    toLog(F("GSM ERROR! Can't init Gsm module!"));
    return false;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  msg(F("Waiting for network..."));
  if (!modem.waitForNetwork())
  {
    delay(1000);
    toLog(F("Can't connect to gsm net..."));
    return false;
  }

  if (modem.isNetworkConnected())
  {
    toLog(F("Network connected"));
  }

  int csq = modem.getSignalQuality();
  toLog("Signal quality:" + String(csq));
  /*
  // This is only supported on SIMxxx series
  String gsmTime = modem.getGSMDateTime(DATE_TIME);
  DBG("GSM Time:", gsmTime);
  String gsmDate = modem.getGSMDateTime(DATE_DATE);
  DBG("GSM Date:", gsmDate);
*/
  String ussd_balance = modem.sendUSSD("*111#");
  toLog(ussd_balance);
  /*
  String ussd_phone_num = modem.sendUSSD("*161#");
  DBG("Phone number (USSD):", ussd_phone_num);
*/
  /*
#if defined(SMS_TARGET)
  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  DBG("SMS:", res ? "OK" : "fail");

  // This is only supported on SIMxxx series
  res = modem.sendSMS_UTF16(SMS_TARGET, u"Привіііт!", 9);
  DBG("UTF16 SMS:", res ? "OK" : "fail");
#endif
*/

  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
  // modem.poweroff();
  // DBG("Poweroff.");

  // Do nothing forevermore
  SerialAT.print(F("AT+CLIP=1\r\n"));

  return true;
}

bool isHangUpCallNumber(String numb = "+380681275553")
{
  toLog("Calling to:" + numb);
  bool res = false;
  SerialAT.print("AT\r\n");
  msg(SerialAT.readString());
  delay(200);
  SerialAT.print(F("AT+CLIP=1\r\n"));
  delay(100);
  res = modem.callNumber(numb);
  msg("Call:" + String(res ? "OK" : "fail"));
  
  if (res) // якщо абонент взяв трубку
  {
    delay(900); //чекаемо 1 сек.
    res = modem.callHangup(); // кладем трубку
    msg("END of call:" + String(res ? "OK" : "fail"));
  }

  /// msg("set decode DTMF!!!");
  ///SerialAT.print(F("AT+DDET=1\r\n"));
  return res;
}

/*
void dtmf(String t="A"){
  SerialAT.print("AT+CDTMF="+t+",500\r\n"); 
  msg("DTMF: "+t+SerialAT.readStringUntil('\n')); 
  delay(700);
}
*/

String getResp()
{
  
  if (!SerialAT.available()) return "";
    String s = "";
    s = SerialAT.readStringUntil('\n');
    // msg("_resp_:"+s);
    s.trim();
   
  return s;
}
String sendAtCommand(String command)
{ 
  String r="", resp = "" ;
  msg("AT"+command);
  SerialAT.print("AT+"+command+"\r\n");
  unsigned long tt=millis();
  
  while (millis()-tt<100){
    r=getResp();
    r.trim();
    if(r!=""){
      resp+=r;
       }
  delay(0);
   
  }/*
  resp="AT"+command+": "+resp;
  delay(10);
  */
  msgIn("run: AT"+command+":_resp: ");msg(resp);
  return resp;
}
void loopGsm()
{

  if (!SerialAT.available())
    return;
  String s = getResp();
  msg(s);
  if (isTelOfUser(s))
  {
    //  msg(sendAtCommand("+DDET = 1"));
    sendAtCommand("A"); //HAND UP

    blinkDi(2000, 20);
    msg(F("HAND DOWN!"));
    sendAtCommand("H"); //HAND DOWN
  }
  // SerialAT.print("AT+CLIP=1\r\n");
  //msg("__MYrec___:"+SerialAT.readStringUntil('\n'));
}