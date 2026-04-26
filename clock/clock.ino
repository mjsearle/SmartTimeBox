// TM Smart Timebox
// 2013 by Mark Searle

#include <TimerOne.h>
#include <PinChangeInt.h>
#include <stdarg.h>
#include <Wire.h>

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

unsigned char time[4] = {0, 0, 0, 0};

// Time threshold for green light to come on.

unsigned char greentime[4] = {0, 0, 5, 0};

struct timearray {
unsigned char storedtime[4];
};

struct timearray storedtimes[64];

static volatile unsigned char stored_count = 0;
static volatile unsigned char storeindex_display = 0;

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
  blankdisplay(1);   // Blank display for 1 microsec. Calling it with argument 0 produced strange behaviour.

// Setup timer for timer interupt every second  
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(tick); // Activate tick ISR each time timer interupt goes off  
  Timer1.stop();   // Then imeaditely stop it so to wait until start button pressed.
   

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
  time[0]++;
  if ( time[0] == 10 ) { time[0] = 0; time[1]++; }
  if ( time[1] == 6 )  { time[1] = 0; time[2]++; }
  if ( time[2] == 10 ) { time[2] = 0; time[3]++; }
  if ( time[3] == 6 )  { time[0] = 0; time[1] = 0; time[2] = 0; time[3] = 0; } 
  sei();   /// Need to renable interupts as I2C needs them.
  // Green time as been reached, switch on green light.
  if ( time[0] == greentime[0] && time[1] == greentime[1] && time[2] == greentime[2] && time[3] == greentime[3] )
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
  else if ( currentmode == 1 ) { currentmode = 4; }
  else { currentmode = 0; }  
}  

// Plus button pushed.
void plusact()
{
  auto unsigned char n = 0;
   if ( currentmode == 0 )
  {
    if ( digitalRead(minusbut) == LOW )
    {
      for ( n = stored_count; n > 0;n-- )
      {
        storedtimes[n] = storedtimes[n - 1];
      }  
      storedtimes[0].storedtime[3] = time[3]; storedtimes[0].storedtime[2] = time[2];
      storedtimes[0].storedtime[1] = time[1]; storedtimes[0].storedtime[0] = time[0];
      if ( stored_count < 63 ) { stored_count++; }
      storeindex_display = 0;
      time[3] = 0; time[2] = 0; time[1] = 0; time[0] = 0;
    }
    else
    {
      Timer1.stop();
    }  
  }
  if ( currentmode == 1 )
  {
    greentime[0]++; 
    if ( greentime[0] == 10 ) { greentime[0] = 0; greentime[1]++; }
    if ( greentime[1] == 6 )  { greentime[1] = 0; greentime[2]++; }
    if ( greentime[2] == 10 ) { greentime[2] = 0; greentime[3]++; }
    if ( greentime[3] == 6 )  { greentime[0] = 0; greentime[1] = 0; greentime[2] = 0; greentime[3] = 0; }  
  }
  if ( currentmode == 4 )
  {
    if ( storeindex_display != 0 ) { storeindex_display--; }   
  }
}  

// Minus button pushed. Make it stall at 00:00 instead of wrapping round.
void minusact()
{
  auto unsigned char n = 0;
   if ( currentmode == 0 )
  {
    if ( digitalRead(plusbut) == LOW )
    {
      for ( n = stored_count; n > 0;n-- )
      {
        storedtimes[n] = storedtimes[n - 1];
      }  
      storedtimes[0].storedtime[3] = time[3]; storedtimes[0].storedtime[2] = time[2];
      storedtimes[0].storedtime[1] = time[1]; storedtimes[0].storedtime[0] = time[0];
      if ( stored_count < 63 ) { stored_count++; }
      storeindex_display = 0;
      time[3] = 0; time[2] = 0; time[1] = 0; time[0] = 0;
    }
    else
    {
      Timer1.resume();
    }  
  }

 if ( currentmode == 1 &&  ( greentime[0] > 0 || greentime[1] > 0 || greentime[2] > 0 || greentime[3] > 0 ) )
 {
    greentime[0]--;    //     *greentime is unsigned, so will wrap around to 0xFF (255) if already 0 then decremented 
    if ( greentime[0] == 255 ) { greentime[0] = 9; greentime[1]--; }
    if ( greentime[1] == 255 )  { greentime[1] = 5; greentime[2]--; }
    if ( greentime[2] == 255 ) { greentime[2] = 9; greentime[3]--; }
    if ( greentime[3] == 255 )  { greentime[3] = 0; greentime[2] = 9; greentime[1] = 5; greentime[0] = 9; }
 }
  if ( currentmode == 4 )
  {
  if ( storeindex_display != stored_count - 1 ) { storeindex_display++; }
  } 
}  
  
  
// Main loop.
void loop() {
  
// Display the time until mode button is pressed.
  while ( currentmode == 0 )
  {  
  write_time(time);  
  }
 // If in set green light time mode.
  if ( currentmode == 1 )
  { 
    auto unsigned char flash = 0;
    while( currentmode == 1 )
    {
      if ( flash < 21 )
      {
	write_time(greentime);
      }
     // Blank the other half of the cycle to create flashing effect.
      else
      {
         blankdisplay(13000);
      }   
    
      if ( flash < 41 ) { flash++; }
      else { flash = 0; p("Hex plusbut_held: %x\n", plusbut_held); p("Hex minusbut_held: %x\n", minusbut_held);  }    // Print out the button held down cycle count for debugging.
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
   if ( currentmode == 4 )
  { 
    auto unsigned char flash = 0;
    while( currentmode == 4 )
    {
      if ( flash < 41 )
      {
        write_time(storedtimes[storeindex_display].storedtime); 
      }
     // Blank the other half of the cycle to create flashing effect.
      else
      {
         blankdisplay(13000);
      }   
    
      if ( flash < 81 ) { flash++; }
      else { flash = 0; } 

    }
  }
 
} 
