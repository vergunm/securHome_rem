#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#include <FS.h>

#include <serviceFunc.h>
#include <pingFunc.h>
#include <gsmFunc.h>
//---from pingFunc-----
extern bool usrOnLine;
extern ipMacInspector *ipMacPrt;
extern byte userCount;
//=====================

bool needOTA = true;
bool isApMode = false;

unsigned int timeOutWiFi = 5 * 1000; // 5 sek
const char *WiFiApSidd = "xm";
const char *WiFiApPwd = "PASS";

const byte OutputPower = 2; // Сила сгналу WiFi [1-20.5] dBm
const char *WiFiApDefSidd = "apEspServ";
const char *WiFiApDefPwd = "12345678a";

const char *www_username = "www_usr";
const char *www_password = "www_pwd";
const char *www_realm = "Повторіть спробу!";
String authFailResponse = "Не вірно введений пароль!";

ESP8266WebServer server(80);
//--------------------------
String getContentType(String filename)
{
  if (server.hasArg("download"))
    return "application/octet-stream";
  else if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
};

bool handleFileRead(String path)
{
  msg("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
};
//------ WiFiLoop-----------
void wiFiLoop()
{
  ArduinoOTA.handle();
  if (needOTA)
    server.handleClient();
} //=====WiFi Loop============

//---------wifi STA-----------------
void initWiFi()
{
  msg(F("starting WIFI STA..."));
  //Serial.setDebugOutput(true);
  unsigned long stTime = millis();
  WiFi.mode(WIFI_STA);
  blinkDi(500, 1);
  WiFi.begin(WiFiApSidd, WiFiApPwd);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && millis() - stTime < timeOutWiFi)
  {
    blinkDi(500, 1);
    msgIn(".");
  }
  IPAddress myIP; // Отримана пристроем ІР адреса

  if (WiFi.status() == WL_CONNECTED)
  {

    toLog("Connected to ");
    toLog(WiFiApDefSidd);
    toLog("IP address: ");
    myIP = WiFi.localIP();
    toLog(WiFi.localIP().toString());
  }
  else
  { //Якщо не підключились до роутера
    toLog(F("fail connect to router!"));
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.setOutputPower(OutputPower);
    /*
        IPAddress Ip(192, 168, 1, 1);
        IPAddress NMask(255, 255, 255, 0);
        WiFi.softAPConfig(Ip, Ip, NMask);
        */
    delay(100);
    WiFi.mode(WIFI_AP);
    delay(100);
    toLog(F("try create AP..."));
    WiFi.softAP(WiFiApDefSidd, WiFiApDefPwd);
    myIP = WiFi.softAPIP();
    toLog(F("AP IP address: "));
    toLog(myIP.toString());
    isApMode = true;
  }
  //==

  if (needOTA)
  {
    toLog(F("Start OTA server!"));
    ArduinoOTA.begin();
  }

  //-------------------
  server.onNotFound([]() {
    if (!server.authenticate(www_username, www_password))
    {
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }
    String json = "",uri = server.uri();
    if (uri == "/getUser")
    {
      if (!server.hasArg("userId"))
        return server.send(500, "text/plain", F("BAD user id!!!"));
      byte id = String(server.arg("userId")).toInt();
      if (id > userCount - 1)
        return server.send(500, "text/plain", F("BAD user id<>!!! "));
      json = "{";
      json += "\"userId\": \"" + String(id) + "\",";
      json += "\"userName\": \"" + ipMacPrt[id].userName + "\",";
      json += "\"userTel\": \"" + ipMacPrt[id].tel + "\",";
      json += "\"userMac\": \"" + ipMacPrt[id].mac + "\"";
      json += "}";
      server.send(200, "application/json", json);
      json = "";
    }
    else if (uri == "/setUser" && server.method() == HTTP_POST)
    {
      if (!server.hasArg("userId"))
        return server.send(500, "text/plain", F("BAD user id!!!"));
      byte id = String(server.arg("userId")).toInt();
      ipMacPrt[id].userName = String(server.arg("userName"));
      ipMacPrt[id].tel = String(server.arg("userTel"));
      ipMacPrt[id].mac = String(server.arg("userMac"));
      server.send(200, "text/plain", F("Saved!"));
    }
    else if (uri == "/sysStatus")
    {
      String activUser = ",\"activUser\":[";
      byte j = 0;
      for (byte i = 0; i < userCount; i++)
      {
        if (ipMacPrt[i].lastOnline)
        {
          activUser += j > 0 ? "," : "";
          activUser += "\"" + ipMacPrt[i].userName + "\"";
          j++;
        }
        msg(activUser);
      }
      activUser += "]";
      json = "{";
      json += "\"heap\":" + String(ESP.getFreeHeap());
      json += ", \"time\":" + String(millis());
      json += ", \"isUsersInHome\":" + String(usrOnLine ? "true" : "false");
      json += ", \"analog\":" + String(analogRead(A0));
      json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
      json += activUser;
      json += "}";
      server.send(200, "application/json", json);
      json = "";
      activUser = "";
    }
    else if (uri == "/dial")
    {
      if (!server.hasArg("tel"))
        return server.send(500, "text/plain", "BAD TEL. number!!!");
      json = "+";
      json += server.arg("tel");
      msgIn(F("GET TEL:"));
      msg(json);
      if(isHangUpCallNumber(json)){return server.send(200, "text/plain", "DODZVONYLYS!"  );}
      else {return server.send(500, "text/plain", "ERROR COLLING!"  );}
      json="";
    }
    if (!handleFileRead(uri))
      server.send(404, "text/plain", "FileNotFound");
  });
   //--
  server.begin();

  msgIn(F("Open http://"));
  msgIn(myIP.toString());
  msg(F("/ in your browser to see it working"));

} //======= WIFI INIT=================