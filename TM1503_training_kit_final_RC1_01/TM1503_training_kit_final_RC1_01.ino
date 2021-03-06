#include <BD7411.h>

//Analog read pins
const int xPin = A3;
const int yPin = A2;
const int zPin = A1;

//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
int minVal = 265;
int maxVal = 402;

//to hold the caculated values
double x;
double y;
double z;

//bluetooth message handler
char RX_Message = 'X';
bool start_flag = false;

  
int hallout_pin = 0; // use D0 pin

BD7411 H1;
BD7411 H2;
BD7411 H3;
BD7411 H4;

//int H1_count=0;
//int H2_count=0;
//int H3_count=0;
//int H4_count=0;

int count = 0;
float t;
float start;
bool H1_on_state=false;
bool H2_on_state=false;
bool H3_on_state=false;
bool H4_on_state=false;

float H1_hall_count=0;
float H2_hall_count=0;
float H3_hall_count=0;
float H4_hall_count=0;

int anchor = false;

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  
  H1.init(18);
  H2.init(19);
  H3.init(20);
  H4.init(21);
  
  start = millis();
  t=start;

//=============================================
//interrupt pin 18-21

  pinMode(18, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(18), abc, FALLING);

  pinMode(19, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(19), abc, FALLING);

    pinMode(20, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(20), abc, FALLING);

    pinMode(21, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(21), abc, FALLING);
}

void abc()
{
  if(!digitalRead(18))
    H1_hall_count+=1.0;
  
  if(!digitalRead(19))
    H2_hall_count+=1.0;

  if(!digitalRead(20))
    H3_hall_count+=1.0;

  if(!digitalRead(21))
    H4_hall_count+=1.0;
}

//=========================================

void bluetooth_control()
{
  while (Serial3.available())
  {
     RX_Message = Serial3.read();    
     if(RX_Message == 'A')
     {
       start_flag = true;
       anchor = false;
       //Serial.print("Start\r\n");
     }else if(RX_Message == 'B')
            {
              start_flag = false;
              //Serial.print("Stop\r\n");
            }
    RX_Message = 'X';
  }  
}

void loop()
{
//gyro sensor work
//read the analog values from the accelerometer
int xRead = analogRead(xPin);
int yRead = analogRead(yPin);
int zRead = analogRead(zPin);

// change the GYRO analog reading to g force

float xvoltage = xRead * 5./1023;
float Gx = (xvoltage - 2.5) / 0.36;
float capture_Gx = 0.0;

float yvoltage = yRead * 5./1023;
float Gy = (yvoltage - 2.5) / 0.36;
float capture_Gy = 0.0;

float zvoltage = zRead * 5./1023;
float Gz = (zvoltage - 2.5) / 0.36;
float capture_Gz = 0.0;


bluetooth_control();

////convert read values to degrees -90 to 90 – Needed for atan2
//int xAng = map(xRead, minVal, maxVal, -90, 90);
//int yAng = map(yRead, minVal, maxVal, -90, 90);
//int zAng = map(zRead, minVal, maxVal, -90, 90);
//
//VoltsRx = 586 * 3.3V / 1023; 
//VoltsRy = 630 * 3.3V / 1023;
//VoltsRz = 561 * 3.3V / 1023;
//
////Caculate 360deg values like so: atan2(-yAng, -zAng)
////atan2 outputs the value of -π to π (radians)
////We are then converting the radians to degrees
//x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
//y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
//z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

//Output the caculations
//Serial.print("x: ");
//Serial.print(x);
//Serial.print(" | y: ");
//Serial.print(y);
//Serial.print(" | z: ");
//Serial.println(z);

//delay(100);//just here to slow down the serial output – Easier to read


//hall sensor counting REV1
  
// set number of hall trips for RPM reading (higher improves accuracy)float hall_thresh = 100.0;
  
  // preallocate values for tach
  
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
 
//Serial.println(H1_hall_count);
  float now = millis();
  if( (now - t) > 100)
  {
    if(start_flag == true)
    {
      Serial3.print ("G: ");
      Serial3.print(Gx);
      Serial3.print (" ");
      Serial3.print(Gy);
      Serial3.print (" ");
      Serial3.println(Gz);
      Serial.print ("G: ");
      Serial.print(Gx);
      Serial.print (" ");
      Serial.print(Gy);
      Serial.print (" ");
      Serial.println(Gz);     
    }else{
           if(anchor==false)
           {
             capture_Gz = Gz;
             capture_Gy = Gy;
             capture_Gx = Gx;
             Serial.print("Capture Gx:");
             Serial.print(capture_Gx);
             Serial.print(" Capture Gy:");
             Serial.print(capture_Gy);
             Serial.print(" Capture Gz:");
             Serial.print(capture_Gz);
             Serial.print("\r\n");

             Serial.print("Cal Gx:");
             Serial.print(Gx - capture_Gx);
             Serial.print(" Cal Gy:");
             Serial.print(Gx - capture_Gy);
             Serial.print(" Cal Gz:");
             Serial.print(Gx - capture_Gz);
             Serial.print("\r\n");
             anchor = true;
           }
         }
    t = now;
    count++;

    if((count % 10 == 0)&&(start_flag == true))
    {
      float end_time = now;
      float time_passed = ((end_time-start)/1000.0);
      Serial3.print("Time Passed: ");
      Serial3.print(time_passed);
      Serial3.println("s");
      Serial.print("Time Passed: ");
      Serial.print(time_passed);
      Serial.println("s");
      
      float rpm_val = (H1_hall_count/time_passed)*60.0;
      Serial3.print(rpm_val);
      Serial3.print(" H1_RPM  ");
      Serial.print(rpm_val);
      Serial.print(" H1_RPM  ");
  
      rpm_val = (H2_hall_count/time_passed)*60.0;
      Serial3.print(rpm_val);
      Serial3.print(" H2_RPM  ");
      Serial.print(rpm_val);
      Serial.print(" H2_RPM  ");
      
      rpm_val = (H3_hall_count/time_passed)*60.0;
      Serial3.print(rpm_val);
      Serial3.print(" H3_RPM  ");
      Serial.print(rpm_val);
      Serial.print(" H3_RPM  ");  
      
      rpm_val = (H4_hall_count/time_passed)*60.0;
      Serial3.print(rpm_val);
      Serial3.println(" H4_RPM");
      Serial.print(rpm_val);
      Serial.println(" H4_RPM");
  
      H1_hall_count=0;
      H2_hall_count=0;
      H3_hall_count=0;
      H4_hall_count=0;
      
      start = now;
    }
  }
}
