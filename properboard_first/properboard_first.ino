#include <TimerOne.h>
#include <PinChangeInt.h>
#include <stdarg.h>
#include <Wire.h>


// Wrapper to printf to serial port for debugging. Remove later.
void p(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600) ;
}

void loop() {
  // put your main code here, to run repeatedly: 
p("Hellow World\n");
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);

delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
delayMicroseconds(32768);
}
