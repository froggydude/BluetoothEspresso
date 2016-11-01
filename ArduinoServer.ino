#include <SoftwareSerial.h>
#include <PID_Beta6.h>
#include <max6675.h>
//
///--------------------todo, modularize, PID, test
//------pin setup
//----pump pin
int pumppin = 3;
//thermocouple pis
int thermoDO = 7;
int thermoCS = 6;
int thermoCLK = 5;
//b2th pin setup(software Serial)
int b2thRX = 11;
int b2thTX = 12;
//---------for temperature processing
double _tempF;
double output, input, _setpoint;
//-------for autobrew
int _PIBT, _PI, _bt;
//-----setup some objects to be used
//thermocouple
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//PID controller
PID espressoTime(&input, &output, &_setpoint, 1, 1, 1);
//imitation serial port for blootooth, tastes just like fruit loops!
SoftwareSerial blu2th(b2thRX, b2thTX);


//---initialize some pins and such
void setup() {
  // put your setup code here, to run once:
  //-----setup some pins
  pinMode(pumppin, OUTPUT);
  /*
  digitalWrite(13, HIGH);
  digitalWrite(13, LOW);//-----------some debuggary*/
  Serial.begin(9600);
  blu2th.begin(9600);
  //initalize setpoint(get from Blu2th later
  _setpoint = 115;
  //-------initalize PID
  //espressoTime.SetMode(1);
  delay(500);


}
boolean resetBit = false;
void loop() {
  //Serial.println(output);                 //debuggary
  //Serial.println("---------------");      //debuggary
  //Serial.println(_tempF);                 //debuggary
  if (blu2th.available()) {
    readBlu2th();
    if (resetBit = false) { //first time its avalable, set resetBit to true
      resetBit = true;
    }
  }
 /* else if (!blu2th.available() && resetBit == true) //if the blu2th is not avalable, and it has been in the past (turn everything off
  
    blu2th.end();           //close connection
    Serial.end()
    resetBit = false;       //reset bit for next time*/                   //think about this one
  
  else {             //otherwise write buffer to bluetooth
    writeBlu2th();
  }

}



void heatControl(double _fromPID) {


}
//supply android with some information from arduino.
void writeBlu2th() {
  char buffet[8] = "";           //buffer char array
  _tempF = upDateTemp();         //get temp from thermocouple
  sprintf(buffet, "%7.3f", _tempF);     //format char array as ---.---
  //blu2th.print(buffet);               //send buffer to android

  delay(100);
  // Serial.println("---------------------------");
}
double upDateTemp()
{

  return thermocouple.readFarenheit();


}
//------------some bluetooth control

void readBlu2th()
{
  byte f;
  String recieved = "";
  f = blu2th.read();
  //-------------switch case for manual/auto
  switch (f)
  {
    //-------------case manual begin pump
    case (2):
      Serial.println("initiate pump");
      digitalWrite(pumppin, HIGH);
      //  delay(100);

      break;
    //--default case, set to pumpin to low(so the pump dosnt keep going)
    case (1):
      digitalWrite(pumppin, LOW);
      //delay(500);
      /* blu2th.print("hello");
       blu2th.print(n);
       blu2th.flush();
       n++;*/
      break;
    //auto brew case
    case (3):
      autoBrew();
      //after autobrew, goback to default, dont keep the pump going
      digitalWrite(pumppin, LOW);
      break;
      //---------if preferences screen has been changed
    case (4):
      blu2thInput();
      break;



    default:
      break;
  }
}
//  _PIBT(time for pump to saturate grinds)
//  _PI(time to wait for brewing)
//  _bt(time to pull a shot)



void autoBrew() {

  _PIBT = _PIBT * 1000;
  _PI = _PI * 1000;
  _bt = _bt * 1000;
  digitalWrite(pumppin, HIGH);  //start pump
  //Serial.println(Serial.available());
  delay(_PIBT);                //for _PIBT ms
  digitalWrite(pumppin, LOW);
  delay(_PI);
  digitalWrite(pumppin, HIGH);
  delay(_bt);
  blu2th.print("done");
}
//sets _PIBT, _PI, _bt, and temperature
void blu2thInput() {
  String b2Re;
    b2Re = blu2th.readStringUntil('>');      //store btstring from android
  String temp;                              //temporary string storage (not to be confused with temperature)
  int i = 0;                               //iterator for going char by char through the string
  char q;                                  //temporary char varaible for storing bytes into String temp
  int w = 1;                             //when a comma (',') is encountered, this controls the switch statment for breaking it up into variables
  int g = 0;                                //another temporary variable for converting from string to int for storage into variabls
  //Serial.println("--------------" + b2Re);     //debuggary
  while (b2Re.length() >= i)                    //iterate though bluetooth string
  {

    q = b2Re[i];                              //store charactor from android string into q
    //------------------case a comma is read, convert to int and set variables accordingly (pibt, pi,bt, stpoint)  
    if (q == ',') {               //if there is a comma, do some processing
      g = temp.toInt();                    //convert temp built string to int
      //Serial.println(g);
      switch (w) {                //switch based on what iteraton of string i.e: "1,2,3,4"
        case (1):
          //first case is PIBT
          _PIBT = g;
          Serial.println(_PIBT);
          w++;
          break;
        case (2):
          //second case is PI
          _PI = g;
          Serial.println(_PI);
          w++;
          break;
          //third case is BT
        case (3):
          _bt = g;
          Serial.println(_bt);
          w++;
          break;
          //forth case is temperature setpoint
        case (4):
          Serial.println(_setpoint);
          _setpoint = g;
          break;
        default:
          break;
      }
      temp = "";   //if the string has been processed, start over
    }
    //----------end comma accorence
    //---otherwise add to temp string
    else {
      temp = temp + q;      //build string char by char
    }

    i++;     //iterate thorugh temp string charbychar
  }
}
