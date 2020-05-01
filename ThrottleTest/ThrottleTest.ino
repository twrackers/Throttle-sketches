#define LOGGING 0

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <HC4LED_Num.h>
#include <StateMachine.h>
#if LOGGING
#include <Streaming.h>
#endif

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *motor = AFMS.getMotor(1);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

const double k_deadband = 24.0;
const int k_period = 40;
const byte k_filter = 5;

double g_throttle = 0.0;
double g_center = 0.0;
bool g_direction = false;
double g_filter[k_filter];

StateMachine pacer(k_period, true);
HC4LED_Num hc4(9);

void setup() 
{
#if LOGGING
  Serial.begin(115200);
#endif
  SPI.begin();
  hc4.blank(false);
  hc4.invert(false);
  pinMode(2, INPUT);

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  motor->setSpeed(0);
  motor->run(FORWARD);

  // Cal throttle lever.
  const int niter = 10;
  for (int i = 0; i < niter; ++i) {
    g_center += (double) analogRead(0);
    delay(100);
  }
  g_center /= niter;

  // Init filter.
  for (int i = 0; i < k_filter; ++i) {
    g_filter[i] = 0.0;
  }

#if LOGGING
  Serial << "cal " << g_center << endl;
#endif
}

void loop() 
{
  if (pacer.update()) {
    
    double raw = g_center - (double) analogRead(0);
    double ctrl = 0.0;
    for (int i = 1; i < k_filter; ++i) {
      ctrl += (g_filter[i - 1] = g_filter[i]);
    }
    ctrl += (g_filter[k_filter - 1] = raw);
    ctrl /= k_filter;
    ctrl = (ctrl >= k_deadband) ? (ctrl - k_deadband) 
         : (ctrl <= -k_deadband) ? (ctrl + k_deadband) 
         : 0.0;
    if (ctrl != 0.0) {
      g_throttle += ctrl / (5000 / k_period);
      g_throttle = constrain(g_throttle, 0.0, 255.0);
    }
#if LOGGING
    Serial << raw << " " << ctrl << " " << g_throttle << endl;
#endif
    
    byte output = (g_throttle > 0.0) ? map((byte) g_throttle, 0, 255, 20, 255) : 0;
    motor->setSpeed(output);
    hc4.writeDec(output);
    
    if (output == 0) {
      g_throttle = 0.0;
      g_direction = (digitalRead(2) == HIGH);
      motor->run(g_direction ? FORWARD : BACKWARD);
    }
    
  }
}
