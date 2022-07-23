#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

//A fist experiences significant acceleration during a good punch,
//followed by massive deceleration at the end of the punch.
//This happens within a fairly small window of time, so it's
//pretty easy to distinguish a punch from normal gesticulations.

unsigned long punchStart = 0;//variable for non-blocking punch timeframe check
const long punchInterval = 500;//timeframe of a punch in ms, from max acceleration to max deceleration, 200 is very generous
int punchAccel = 20;//the beginning of a punch in m/s^2, could be over 50m/s^2 depending on the puncherp
int punchDecel = 15;//the end of a punch in m/s^2, could be less than -100m/s^2 depending on the puncher

// ^^^^^ CHANGE TO -40??????

int flameTime = 250;//how long the flame lasts, in ms

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  //Test to see if accelerometer is communicating
  Serial.begin(9600);
  Serial.println("LIS3DH test!");  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_16_G);   //+-16G range for good punch detection
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");
  
  pinMode(9, OUTPUT); //Solenoid valve
  pinMode(7, OUTPUT); //Arc lighter
  digitalWrite(9, LOW);
  digitalWrite(7, LOW);
}

void loop() {
  lis.read();
  sensors_event_t event; 
  lis.getEvent(&event);
  Serial.println("Acceleration x | y | z ");
  Serial.print(event.acceleration.x);
  Serial.print(" ");
  Serial.print(event.acceleration.y);
  Serial.print(" ");
  Serial.print(abs(event.acceleration.z));
  Serial.println("-----------------------");
  
 //look for punch starting, at least 20 m/s^2
  if (abs(event.acceleration.z) > punchAccel){
      Serial.print("punch in progress");
     Serial.println(abs(event.acceleration.z));
     punchStart = millis();
  }
  
  unsigned long currentMillis = millis();

  //look for punch ending, less than -40 m/s^2
  if (abs(event.acceleration.z) < punchDecel && currentMillis - punchStart < punchInterval){
    
      Serial.println(abs(event.acceleration.z));
      Serial.println("Punch ended");
      Fire(flameTime);
      delay(1000);
    }
}




void Fire(int flameTime){

  digitalWrite(9, HIGH);
  digitalWrite(7, HIGH);
  delay(flameTime);
  digitalWrite(9, LOW);
  digitalWrite(7, LOW);
}
