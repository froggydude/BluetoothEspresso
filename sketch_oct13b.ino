#include <SoftwareSerial.h>
#include <PID_Beta6.h>
#include <max6675.h>

//------pin setup
//----pump pin
int pumppin = 13;
//thermocouple pis
int thermoDO = 7;
int thermoCS = 6;
int thermoCLK = 5;
//b2th pin setup(software Serial)
int b2thRX = 11;
int b2thTX = 12;
//---------for temperature processing
int _tempF;
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

int d = 0;
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
void loop() {
  // put your main code here, to run repeatedly:
  //-------_tempF gets passed to PID Blu2th for GOOi
 // _tempF = upDateTemp();
  //espressoTime.Compute();
  //Serial.println(output);                 //debuggary
  //Serial.println("---------------");      //debuggary
  //Serial.println(_tempF);                 //debuggary
  //heatControl(output);
  //delay(100);
  if (blu2th.available()) {
    readBlu2th();
  }
  else {
    writeBlu2th();
  }
}



void heatControl(double _fromPID) {


}
//supply android with some information from arduino.
int n;

void writeBlu2th() {

  String _tx;
  int conv = (int)upDateTemp();
  char buffet[4] = "";
  _tx = String(upDateTemp());
  Serial.println(_tx);
 sprintf(buffet, "%04d",conv);

     blu2th.print(buffet);
    blu2th.flush();
  delay(100);
  // Serial.println("---------------------------");
  //delay(100);
  // n++;
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
  // n++;
  // if (blu2th.available())
  //  {
  f = blu2th.read();
  //Serial.println("SerAval");
  //}

  switch (f)
  {
    case (2):

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
    //auto
    case (3):
      autoBrew();
      // digitalWrite(13,LOW);
      // f = 1;  //after autobrew, goback to default
      digitalWrite(pumppin, LOW);
      break;
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
  //digitalWrite(pumppin,LOW); //turn off pump
  // blu2th.print("done");
}
//sets _PIBT, _PI, _bt, and temperature
void blu2thInput() {
  String b2Re;
  if (blu2th.available()) {
    b2Re = blu2th.readStringUntil('>');
  }
  String temp;
  int i = 0;
  char q;
  int w = 1;
  int g = 0;
  Serial.println("--------------" + b2Re);
  while (b2Re.length() >= i)
  {

    // Serial.println(_s.length());
    //Serial.println(i);
    q = b2Re[i];
    //  Serial.println(q);
    //------------------case a comma is read, convert to int and set variables accordingly
    if (q == ',') {
      g = temp.toInt();
      //Serial.println(g);
      switch (w) {
        case (1):
          _PIBT = g;
          Serial.println(_PIBT);
          w++;
          break;
        case (2):
          _PI = g;
          Serial.println(_PI);
          w++;
          break;
        case (3):
          _bt = g;
          Serial.println(_bt);
          w++;
          break;
        case (4):
          Serial.println(_setpoint);
          _setpoint = g;
          break;
        default:
          break;
      }
      temp = "";
    }
    //----------end comma accorence
    //---otherwise add to temp string
    else {
      temp = temp + q;
    }

    i++;
  }
}
