// TM Smart Timebox
// 2013,2014,2015,2016,2026 by Mark Searle

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// Arguments for LED Setting Functions
#define ALLOFF 0
#define GREEN 1
#define GREEN_BLINK 2
#define YELLOW 3
#define YELLOW_BLINK 4
#define RED 5
#define RED_BLINK 6


#include <TimerOne.h>
#include <PinChangeInt.h>
#include <stdarg.h>
#include <Wire.h>

// Setup Pin Change interupts
uint8_t latest_interrupted_pin;
uint8_t interrupt_count[20]={0}; // 20 possible arduino pins

// 7 Segment, common cathodes. Set high to make the digit display
int digit1 = 10; 
int digit2 = A2; 
int digit3 = A1; 
int digit4 = A0; 

// 7 Segment Anodes. Drive low to make segment display
int segA = 3;
int segB = 4;
int segC = 9;
int segD = 7;   // Handwired change from RevA circuit board layout
int segE = 8;   // ditto 
int segF = A3;
int segG = 2;

int preset1 = 5;
int preset2 = 6;

int modebut = 13;
int plusbut = 12;
int minusbut= 11;

// Master TimeKeeping variable, This is the current time of the stopwatch

unsigned char time[4] = {0, 0, 0, 0};

// Time thresholds for green, yellow and red lights to come on. Default to higest value so lights don't come on when timing intros and evaluations etc

unsigned char greentime[4] = {0, 0, 3, 1};
unsigned char yellowtime[4] = {0, 0, 4, 1};
unsigned char redtime[4] = {0, 0, 5, 1};

struct timearray {
  unsigned char storedtime[4];
};

struct preset_array {
  struct timearray preset_times[3];
};



struct timearray storedtimes[64];   // Store the time on the stopwatch each time it is reset

struct preset_array standard_times[21] = {
                                           {{ {{0, 5, 0, 0}} , {{5, 5, 0, 0}} , {{0, 0, 1, 0}} }}, 
                                           {{ {{0, 0, 1, 0}} , {{0, 3, 1, 0}} , {{0, 0, 2, 0}} }},
                                           {{ {{5, 4, 1, 0}} , {{0, 2, 2, 0}} , {{0, 0, 3, 0}} }},
                                           {{ {{0, 0, 2, 0}} , {{0, 0, 3, 0}} , {{0, 0, 4, 0}} }},
                                           {{ {{0, 0, 3, 0}} , {{0, 0, 4, 0}} , {{0, 0, 5, 0}} }},
                                           {{ {{0, 0, 4, 0}} , {{0, 0, 5, 0}} , {{0, 0, 6, 0}} }},
                                           {{ {{0, 0, 5, 0}} , {{0, 0, 6, 0}} , {{0, 0, 7, 0}} }},
                                           {{ {{0, 0, 6, 0}} , {{0, 0, 7, 0}} , {{0, 0, 8, 0}} }},
                                           {{ {{0, 0, 7, 0}} , {{0, 0, 8, 0}} , {{0, 0, 9, 0}} }},
                                           {{ {{0, 0, 8, 0}} , {{0, 0, 9, 0}} , {{0, 0, 0, 1}} }},
                                           {{ {{0, 0, 9, 0}} , {{0, 0, 0, 1}} , {{0, 0, 1, 1}} }},
                                           {{ {{0, 0, 0, 1}} , {{0, 0, 1, 1}} , {{0, 0, 2, 1}} }},
                                           {{ {{0, 0, 1, 1}} , {{0, 0, 2, 1}} , {{0, 0, 3, 1}} }},
                                           {{ {{0, 0, 2, 1}} , {{0, 0, 3, 1}} , {{0, 0, 4, 1}} }},
                                           {{ {{0, 0, 3, 1}} , {{0, 0, 4, 1}} , {{0, 0, 5, 1}} }},
                                           {{ {{0, 0, 4, 1}} , {{0, 0, 5, 1}} , {{0, 0, 6, 1}} }},
                                           {{ {{0, 0, 5, 1}} , {{0, 0, 6, 1}} , {{0, 0, 7, 1}} }},
                                           {{ {{0, 0, 6, 1}} , {{0, 0, 7, 1}} , {{0, 0, 8, 1}} }},
                                           {{ {{0, 0, 7, 1}} , {{0, 0, 8, 1}} , {{0, 0, 9, 1}} }},                                           
                                           {{ {{0, 0, 8, 1}} , {{0, 0, 9, 1}} , {{0, 0, 0, 2}} }},                                           
                                           {{ {{0, 0, 3, 2}} , {{0, 0, 4, 2}} , {{0, 0, 5, 2}} }}, 
                                         }; 
                                         
struct preset_array but_preset_times[8] = {
                                           {{ {{0, 0, 1, 0}} , {{0, 3, 1, 0}} , {{0, 0, 2, 0}} }},     // 2 min TT         0
                                           {{ {{5, 4, 1, 0}} , {{0, 2, 2, 0}} , {{0, 0, 3, 0}} }},     // 3 min segment    1 
                                           {{ {{0, 0, 5, 0}} , {{0, 0, 6, 0}} , {{0, 0, 7, 0}} }},      // 7 min speech    2 
                                           {{ {{0, 0, 3, 1}} , {{0, 0, 4, 1}} , {{0, 0, 5, 1}} }},      // 15 min          3
                                           {{ {{0, 5, 0, 0}} , {{5, 5, 0, 0}} , {{0, 0, 1, 0}} }},      // 1 min TT        4 
                                           {{ {{0, 0, 3, 0}} , {{0, 0, 4, 0}} , {{0, 0, 5, 0}} }},      // 5min speech     5
                                           {{ {{0, 0, 7, 0}} , {{0, 0, 8, 0}} , {{0, 0, 9, 0}} }},     // 9 min speech     6
                                           {{ {{0, 0, 8, 0}} , {{0, 0, 9, 0}} , {{0, 0, 0, 1}} }},      // 10 min speech   7
                                         }; 


static volatile unsigned char stored_count = 0;
static volatile unsigned char storeindex_display = 0;

// Keep tally if plus or minus buttons are held down
int plusbut_held;
int minusbut_held;
int modebut_held;

unsigned char brightness_level = 1;  // Brightness setting has six allowed values. 0 least bright, 5 highest brightness. Level 1 is the default

static volatile unsigned int brightness_on_time;
static volatile unsigned int brightness_off_time;


unsigned char flash = 0;   // Used by main loop for generating flashing effect

static volatile unsigned char currentmode = 0;

static volatile unsigned char current_standardtime = 13;
static volatile unsigned char current_presettime = 3;

// 0  = Displaying Stopwatch. 1  = Displaying/Setting Green Light Time. 2  = Displaying/Setting  Yellow Time. 3  = Displaying/Setting Red time.
// 4 = Displaying previous speech times. 5  = Setting brightness.

static volatile unsigned char ten_mm_ledstate = 0;

// 0 no 10mm LED on. 1 green on, 2 yellow on, 3 red on

static volatile unsigned char preset_just_pressed = 0;
static volatile unsigned char saw_but_release = 0;
static unsigned long last_preset_time = 0;

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
void setup() 
{      
// set prescale to 8 to speed up analogue reads. Interupts not available on ADC6 and 7 which are connected to preset buttons 4 and 3 respectively.
// hence the need to poll them. Consequent loss of resolution is not an issue as only looking for button presses.
cbi(ADCSRA,ADPS2);
sbi(ADCSRA,ADPS1);
sbi(ADCSRA,ADPS0);
 
LED_SetupBlink();   // Sets period of PWM1 in PCA9552 to ~ 0.8 sec. Sets blink rate of 3mm LEDs 
                    // Also sets period of PWM0 to 1.6 sec, sets blink rate of 10mm LEDs
                    
// Setup pin modes  
  pinMode(digit1,OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  pinMode(modebut, INPUT);
  pinMode(plusbut, INPUT);
  pinMode(minusbut, INPUT);
  
  pinMode(preset1, INPUT);
  pinMode(preset2, INPUT);
   
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);  
  pinMode(segG, OUTPUT); 
  
// Attach interupts functions to the three main buttons, and the first two preset buttons  
  PCintPort::attachInterrupt(modebut, &modeselect, FALLING);
  PCintPort::attachInterrupt(plusbut, &plusact, FALLING);
  PCintPort::attachInterrupt(minusbut, &minusact, FALLING);
  PCintPort::attachInterrupt(preset1, &preset1act, FALLING);
  PCintPort::attachInterrupt(preset2, &preset2act, FALLING);

// Switch off all segments  
  blankdisplay(1);   // Blank display for 1 microsec. Calling it with argument 0 produced strange behaviour.
  
  ActuateBrightness(brightness_level); // Set the brightness to default value

// Setup timer for timer interupt every second  
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(tick); // Activate tick ISR each time timer interupt goes off  
  Timer1.stop();   // Then imeaditely stop it so to wait until start button pressed.
   

// Initialse serial port for debugging. Remove later.  
  Serial.begin(9600) ;

// Ensure all three lights are initially off.   
  LED_TenMM(ALLOFF);
  LED_ThreeMM(ALLOFF);
}  
  
// Main loop.
void loop() {
 
  
// Display the time until mode button is pressed.
 if ( currentmode == 0 )
  flash = 0;
  while ( currentmode == 0 )
  {  
    if ( flash == 10 ) 
    {
        // Time out the preset just pressed flag
        if (preset_just_pressed && ((millis() - last_preset_time > 400))) {
            preset_just_pressed = 0;
        }
      CheckAnalogSixandSeven();
    } 
    write_time(time);
    if ( flash < 11 ) { flash++; }
    else { flash = 0; } 
  }
 // If in set green light time mode (simplfied).
  if ( currentmode == 1 )
  { 
    flash = 0;
    while( currentmode == 1 )
    {
      TimeColourWrite_Simplified(greentime);
      if ( flash < 41 ) { flash++; }
      else { flash = 0; }   
      if ( digitalRead(modebut) == LOW )  {  modebut_held++; } // Notice that the mode button is being held down
      else { modebut_held = 0; } // Want to detect prolonged holding of mode button only
      if ( modebut_held == 120 )
      {
        modebut_held = 0;
        currentmode = 4; // Jump to "set arbitary light time modes" starts at 4 for set green arbitary 
      }             
    }
  }
   // If in set yellow light time mode (simplified).
  if ( currentmode == 2 )
  {
    flash = 0;
    while( currentmode == 2 )
    {
      TimeColourWrite_Simplified(yellowtime);
      if ( flash < 41 ) { flash++; }
      else { flash = 0; }   
    }
  }
     // If in set red light time mode (simplified).
  if ( currentmode == 3 )
  {
    flash = 0; 
    while( currentmode == 3 )
    {
      TimeColourWrite_Simplified(redtime);
      if ( flash < 41 ) { flash++; }
      else { flash = 0; }   
    }
  }
  
 // If in set green light time mode (arbitary).
  if ( currentmode == 4 )
  { 
    flash = 0;
    while( currentmode == 4 )
    {
      TimeColourWrite(greentime);
      if ( flash < 31 ) { flash++; }
      else { flash = 0; }   
      if ( digitalRead(modebut) == LOW )  {  modebut_held++; } // Notice that the mode button is being held down
      else { modebut_held = 0; } // Want to detect prolonged holding of mode button only
      if ( modebut_held == 200 )
      {
        modebut_held = 0;
        currentmode = 7; // Jump to "special" modes 7 and 8 
      }             
    }
  }
   // If in set yellow light time mode (aribtary).
  if ( currentmode == 5 )
  {
    flash = 0;
    while( currentmode == 5 )
    {
      TimeColourWrite(yellowtime);
      if ( flash < 31 ) { flash++; }
      else { flash = 0; }   
    }
  }
     // If in set red light time mode (aribitary).
  if ( currentmode == 6 )
  {
    flash = 0; 
    while( currentmode == 6 )
    {
      TimeColourWrite(redtime);
      if ( flash < 31 ) { flash++; }
      else { flash = 0; }   
    }
  }
  if ( currentmode == 7 )
  { 
    flash = 0;
    LED_ThreeMM(ALLOFF);
    while( currentmode == 7 )
    {
      if ( flash == 10 || flash == 20 || flash == 30 || flash == 40 || flash == 50 || flash == 60 || flash == 70 || flash == 80) 
      {
        CheckAnalogSixandSeven();
      } 
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
  
  if ( currentmode == 8 )
  { 
    flash = 0;
    blankdisplay(1);
    while( currentmode == 8 )
    {
      if ( flash == 10 ) 
      {
        CheckAnalogSixandSeven();
      } 
      // Do some low level magic here
      digitalWrite(digit4, LOW);
      writeletterb(); 
      delayMicroseconds(brightness_off_time);
      digitalWrite(digit1, HIGH);
      delayMicroseconds(brightness_on_time);

      digitalWrite(digit1, LOW);
      writeblank(); 
      delayMicroseconds(brightness_off_time);
      digitalWrite(digit2, HIGH);
      delayMicroseconds(brightness_on_time);

      digitalWrite(digit2, LOW);
      writeblank(); 
      delayMicroseconds(brightness_off_time);
      digitalWrite(digit3, HIGH);
      delayMicroseconds(brightness_on_time);

      write_digit(brightness_level,digit3,digit4);   
      if ( flash < 21 ) { flash++; }
      else { flash = 0; } 
    }
  }
}  
  

