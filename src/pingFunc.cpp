
/*****************************************************************************
Arduino library handling ping messages for the esp8266 platform

MIT License

Copyright (c) 2018 Alessio Leoncini

*****************************************************************************/
#include <Arduino.h>
#include <Pinger.h>
#include <serviceFunc.h>
#include <IPAddress.h>
#include <pingFunc.h>
#include <wiFiFunc.h>
#include <fsReadWrite.h>

//-------TEMP TIME VAR---
unsigned long unixTime=0;
//------------------------
byte ipFrom = 2, ipTo = 6;        // діапазон ІР які будуть скануватись
const byte COUNT_USERS = 4;       //Максимальна кількість юзерів в ситемі
byte ipNet[4] = {192, 168, 4, 1}; //зразок ip який буде скануватись
byte userCount = COUNT_USERS;
//------isUsersOnLine ------
bool usrOnLine = false;

void chekUsersOnLine();

//---------------
ipMacInspector ipMac1[COUNT_USERS] = {};
ipMacInspector *ipMacPrt = ipMac1;
//=============================
//-----
//--- ADD NEW USER-----
void addUser(String userName, String mac, String tel)
{
  static byte iMac = 0;
  ipMac1[iMac].userName = userName;
  ipMac1[iMac].mac = mac;
  ipMac1[iMac].tel = tel;
  msgIn(F("Create new user: "));
  msgIn(ipMac1[iMac].userName);
  msgIn(" MAC: ");
  msgIn(ipMac1[iMac].mac);
  toLog("ADD new user: " + userName + " MAC:" + mac + " tel" + tel);
  msg("==========user==========");
  iMac += iMac < COUNT_USERS ? 1 : 0;
  
}

extern "C"
{
#include <lwip/icmp.h> // needed for icmp packet definitions
}

//ipMacInspector  *prtIpMac=ipMac;

Pinger pinger;

//=========================

String macToString(const unsigned char *mac)
{
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

void initPing()
{
  pinger.OnReceive([](const PingerResponse &response) { return true; });

  pinger.OnEnd([](const PingerResponse &response) {
    msgIn(F("IP address: "));
    msg(String(response.DestIPAddress.toString().c_str()));
    if (response.DestMacAddress != nullptr)
    {
      //  Serial.printf("    MAC address: " MACSTR "\n", MAC2STR(response.DestMacAddress->addr));
      String s = macToString(response.DestMacAddress->addr);
      // String( response.DestMacAddress->addr[0],HEX);
      msgIn(F("MAC: "));
      msg(s);
      // msgIn( "TEST MAC: "); toLog(s);
      // ipMacPrt[i_IpMac].mac=String(s);

      for (byte ii = 0; ii < COUNT_USERS; ii++)
      { //
        String usrMac = ipMacPrt[ii].mac;
        if (usrMac.equalsIgnoreCase(macToString(response.DestMacAddress->addr)))
        {
          msgIn("USER ");
          msgIn(ipMacPrt[ii].userName);
          msg(" - ONLINE!");
          usrMac = ipMacPrt[ii].isOnLine = true;
          usrOnLine = true;
        }
      }

      // strncpy(ipMacPrt[i_IpMac].mac_c,macToStr_c(response.DestMacAddress->addr),20);
    }
    else
    {
      msg(F("------NO MAC!-----"));
    }

    //============================
    return true;
  });
}

bool isUserInHome()
{
  for (byte ii = 0; ii < COUNT_USERS; ii++)
  {
    if (ipMacPrt[ii].lastOnline)
    {
      return true;
    }
  }
  return false;
}
void clearStatusUser()
{
  for (byte ii = 0; ii < COUNT_USERS; ii++)
  {
    ipMacPrt[ii].lastOnline = ipMacPrt[ii].isOnLine;
    ipMacPrt[ii].isOnLine = false;
  }
}
//--------chekUsersOnLine---------
void chekUsersOnLine()
{
  static byte iip = ipFrom;
  static String sLog = "";
  pinger.Ping(IPAddress(ipNet[0], ipNet[1], ipNet[2], iip), 1, 100);
  if (ipMac1[iip].lastOnline != ipMac1[iip].isOnLine)
  {
    sLog+=String(unixTime)+";";
    sLog+=String(iip)+"/n" ;
    
  }
  if (iip == ipTo)
  {
    iip = ipFrom;
    usrOnLine = isUserInHome();
    clearStatusUser();
  }
  else
  {

    iip++;
  };
  msgIn(F("IsUser in home:"));
  msg(String(usrOnLine ? "True" : "False"));
}
//-------isTelOfUser---------------
bool isTelOfUser(String tel)
{
  for (byte i = 0; i < COUNT_USERS; i++)
  {
    if (tel.indexOf(ipMac1[i].tel) > 0)
    {
      msg("FIND TEL USER" + tel);
      return true;
    }
  }
  return false;
}