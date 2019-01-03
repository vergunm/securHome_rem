#include <Arduino.h>
#include <serviceFunc.h>

const int DOOR_PIN=8;
 
//---- doorOpen-----
void doorOpen () {
  
pinMode(DOOR_PIN, OUTPUT);
}
//======doorOpen==========
//---- doorClos-----
void doorClose(){

}
//======doorClos==========

void initDoor(){
    pinMode(DOOR_PIN, OUTPUT);
    doorClose();
}
