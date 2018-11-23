#ifndef PING_FUNC_h
#define PING_FUNC_h
#include <IPAddress.h>
void initPing(void);
bool pingIp(IPAddress);
 

struct ipMacInspector  {
  String userName="Name";
  String tel="380681231212";
  String mac="ac:ac:ac:ac:ac";
  bool lastOnline=false;
  bool isOnLine=false;
} ;

void addUser(String userName,String mac, String tel );
 
void chekUsersOnLine();
bool isTelOfUser(String tel);
#endif