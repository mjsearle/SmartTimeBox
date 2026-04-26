// TM Smart Timebox
// 2013 by Mark Searle

#include <TimerOne.h>
#include <PinChangeInt.h>
#include <stdarg.h>
#include <Wire.h>
#include "sevseg.h"

// Setup Pin Change interupts
uint8_t latest_interrupted_pin;
uint8_t interrupt_count[20]={0}; // 20 possible arduino pins

// 7 Segment, common cathodes. Set low to make digit display
int digit1 = A0;
int digit2 = A1;
int digit3 = 9;
int digit4 = 10;

// 7 Segment Anodes
int segA = 3;
int segB = 4;
int segC = 13;
int segD = 11;
int segE = 12;
int segF = A3;
int segG = 2;

int modebut = 7;
int plusbut = 8;
int minusbut= A2;

char time0 = 0;
char time1 = 0;
char time2 = 0;
char time3 = 0;

// Time threshold for green light to come on.
char greentime0 = 0;
char greentime1 = 0;
char greentime2 = 5;
char greentime3 = 0;

char storedtimes0[10];
char storedtimes1[10];
char storedtimes2[10];
char storedtimes3[10];

static volatile char storeindex = 0;
static volatile char storeindex_display = 0;

// Keep tally if plus or minus buttons are held down
int plusbut_held;
int minusbut_held;

static volatile unsigned char currentmode = 0;

// 0  = Displaying Stopwatch. 1  = Displaying/Setting Green Light Time. 2 Displaying/Setting  Yellow Time. 3 Displaying/Setting Red time.
// 4 = Displaying previous speech times.

// Handler for pin change interputs.

void quicfunc() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  interrupt_count[latest_interrupted_pin]++;
}

// Wrapper to printf to serial port for debugging. Remove later.
void p(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}



// the setup routine runs once when you press reset:
void setup() {               

// Setup pin modes 
  pinMode(digit1,OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  pinMode(modebut, INPUT);
  pinMode(plusbut, INPUT);
  pinMode(minusbut, INPUT);

  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT); 
  pinMode(segG, OUTPUT);
 
// Attach interupts functions to the three main buttons 
  PCintPort::attachInterrupt(modebut, &modeselect, FALLING);
  PCintPort::attachInterrupt(plusbut, &plusact, FALLING);
  PCintPort::attachInterrupt(minusbut, &minusact, FALLING);
 
// Switch off all segments 
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);


  Timer1.initialize(1000000);       // Setup timer for timer interupt every second 
  Timer1.attachInterrupt(tick);    // Activate tick ISR each time timer interupt goes off 
  Timer1.stop();
  

// Initialse serial port for debugging. Remove later. 
  Serial.begin(9600) ;

// Ensure all three lights are initially off. PCA9552 LED driver on i2c bus at address 0x67. 
  Wire.begin();
  Wire.beginTransmission(0x67);
  Wire.write(0x08);
  Wire.write(0x55);
  Wire.endTransmission();
}

void tick()
{
  time0++;
  if ( time0 == 10 ) { time0 = 0; time1++; }
  if ( time1 == 6 )  { time1 = 0; time2++; }
  if ( time2 == 10 ) { time2 = 0; time3++; }
  if ( time3 == 6 )  { time0 = 0; time1 = 0; time2 = 0; time3 = 0; }
  sei();   /// Need to renable interupts as I2C needs them.
  // Green time as been reached, switch on green light.
  if ( time0 == greentime0 && time1 == greentime1 && time2 == greentime2 && time3 == greentime3 )
  {
    Wire.beginTransmission(0x67);
    Wire.write(0x08);
    Wire.write(0x00);
    Wire.endTransmission();
  }
}

// Cycle between modes 0 and 1. Implement 2, 3 and 4 later.
void modeselect()
{
  if ( currentmode == 0 ) { currentmode = 1; }
  else if ( currentmode == 1 ) { currentmode = 5; }
  else { currentmode = 0; } 
} 

// Plus button pushed.
void plusact()
{
   if ( currentmode == 0 )
  {
    if ( digitalRead(minusbut) == LOW )
    {
      storedtimes3[storeindex] = time3; storedtimes2[storeindex] = time2;
      storedtimes1[storeindex] = time1; storedtimes0[storeindex] = time0;
      storeindex++; if ( storeindex == 10 ) { storeindex = 0; };
      storeindex_display = storeindex; storeindex_display--;
      time3 = 0; time2 = 0; time1 = 0; time0 = 0;
    }
    else
    {
      Timer1.stop();
    } 
  }
  if ( currentmode == 1 )
  {
    greentime0++;
    if ( greentime0 == 10 ) { greentime0 = 0; greentime1++; }
    if ( greentime1 == 6 )  { greentime1 = 0; greentime2++; }
    if ( greentime2 == 10 ) { greentime2 = 0; greentime3++; }
    if ( greentime3 == 6 )  { greentime0 = 0; greentime1 = 0; greentime2 = 0; greentime3 = 0; } 
  }
  if ( currentmode == 5 )
  {
    if ( storeindex_display != storeindex - 1 ) { storeindex_display++; }  
  }
} 

// Minus button pushed. Make it stall at 00:00 instead of wrapping round.
void minusact()
{
   if ( currentmode == 0 )
  {
    if ( digitalRead(plusbut) == LOW )
    {
      storedtimes3[storeindex] = time3; storedtimes2[storeindex] = time2;
      storedtimes1[storeindex] = time1; storedtimes0[storeindex] = time0;
      storeindex++; if ( storeindex == 10 ) { storeindex = 0; };
      storeindex_display = storeindex; storeindex_display--;
      time3 = 0; time2 = 0; time1 = 0; time0 = 0;
    }
    else
    {
      Timer1.resume();
    } 
  }

  if ( currentmode == 1 &&  ( greentime0 > 0 || greentime1 > 0 || greentime2 > 0 || greentime3 > 0 ) )
 {
    greentime0--;
    if ( greentime0 == -1 ) { greentime0 = 9; greentime1--; }
    if ( greentime1 == -1 )  { greentime1 = 5; greentime2--; }
    if ( greentime2 == -1 ) { greentime2 = 9; greentime3--; }
    if ( greentime3 == -1 )  { greentime3 = 0; greentime2 = 9; greentime1 = 5; greentime0 = 9; }
 }
  if ( currentmode == 5 )
  {
  if ( storeindex_display == 0 ) { storeindex_display = storeindex - 1; }
  else { storeindex_display--; } 
  }
} 
 
 
// Main loop.
void loop() {
 
// Display the time until mode button is pressed.
  while ( currentmode == 0 )
  { 
  write_digit(time3,digit4,digit1);
  write_digit(time2,digit1,digit2);
  write_digit(time1,digit2,digit3);
  write_digit(time0,digit3,digit4); 

  }
 // If in set green light time mode.
  if ( currentmode == 1 )
  {
    auto unsigned char flash = 0;
    while( currentmode == 1 )
    {
      if ( flash < 21 )
      {
	write_digit(greentime3,digit4,digit1);
	write_digit(greentime2,digit1,digit2);
	write_digit(greentime1,digit2,digit3);
	write_digit(greentime0,digit3,digit4);
      }
     // Blank the other half of the cycle to create flashing effect.
      else
      {
         digitalWrite(digit1, HIGH);
         digitalWrite(digit2, HIGH);
         digitalWrite(digit3, HIGH);
         digitalWrite(digit4, HIGH);
         delayMicroseconds(13000);
      }  
   
      if ( flash < 41 ) { flash++; }
      if ( flash == 41 ) { flash = 0; p("Hex plusbut_held: %x\n", plusbut_held); }    // Print out the button held down cycle count for debugging.
      if ( digitalRead(plusbut) == LOW )
      {
          plusbut_held++;
      }
      else
      {  
         plusbut_held = 0;     // Set the count to zero once button released, don't bother counting as next push will be picked up by interupt.
      }
      if ( plusbut_held > 800 ) { plusact(); plusact(); plusact(); }   // Superfast scan mode
      else if ( plusbut_held > 200 ) { plusact(); plusact(); }    // Fast scan mode
      else if ( plusbut_held > 40 ) { plusact(); }      // Standard scan mode

      if ( digitalRead(minusbut) == LOW )
      {
          minusbut_held++;
      }
      else
      {  
         minusbut_held = 0;
      }
      if ( minusbut_held > 800 ) { minusact(); minusact(); minusact(); } 
      else if ( minusbut_held > 200 ) { minusact(); minusact(); }
      else if ( minusbut_held > 40 ) { minusact(); }
    }
  }
   if ( currentmode == 5 )
  {
    auto unsigned char flash = 0;
    while( currentmode == 5 )
    {
      if ( flash < 11 )
      {
	write_digit(storedtimes3[storeindex_display],digit4,digit1);
	write_digit(storedtimes2[storeindex_display],digit1,digit2);
	write_digit(storedtimes1[storeindex_display],digit2,digit3);
	write_digit(storedtimes0[storeindex_display],digit3,digit4);
      }

     // Blank the other half of the cycle to create flashing effect.
      else
      {
         digitalWrite(digit1, HIGH);
         digitalWrite(digit2, HIGH);
         digitalWrite(digit3, HIGH);
         digitalWrite(digit4, HIGH);
         delayMicroseconds(13000);
      }  
   
      if ( flash < 21 ) { flash++; }
      if ( flash == 21 ) { flash = 0; }

    }
  }
 
} 
