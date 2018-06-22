#include "SR04.h"
#define TRIG_PIN 12
#define ECHO_PIN 11

#define ECHO_PIN2 13

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
SR04 sr04_2 = SR04(ECHO_PIN2,TRIG_PIN);
long a;
long b;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); 
}

void loop() {
  // put your main code here, to run repeatedly:
   a=sr04.Distance();
   Serial.print(a);
   Serial.println("cm");
   b=sr04_2.Distance();
   Serial.print(b);
   Serial.println("cm");
   delay(1000);
   
}
