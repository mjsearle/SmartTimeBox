// Setup functions to write each number
void writezero() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, HIGH);
}


void writeone() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writetwo() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, LOW);
}

void writethree() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, LOW);
}

void writefour() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writefive() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writesix() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writeseven() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writeeight() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writenine() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}
// Use digit 10 to write a blank
void writeblank() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}
// Use digit 11 to write a b
void writeletterb() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}


void write_digit( unsigned char time_digit, int prev_digit, int curr_digit )
{
  digitalWrite(prev_digit, LOW);
  if (time_digit == 0 ) {   writezero(); }
  if (time_digit == 1 ) {   writeone(); }
  if (time_digit == 2 ) {   writetwo(); }
  if (time_digit == 3 ) {   writethree(); }
  if (time_digit == 4 ) {   writefour(); }
  if (time_digit == 5 ) {   writefive(); }
  if (time_digit == 6 ) {   writesix(); }
  if (time_digit == 7 ) {   writeseven(); }
  if (time_digit == 8 ) {   writeeight(); }
  if (time_digit == 9 ) {   writenine(); }
  delayMicroseconds(brightness_off_time);
  digitalWrite(curr_digit, HIGH);
  delayMicroseconds(brightness_on_time);
}

void write_time( unsigned char *time )
{
  write_digit( time[3], digit4, digit1 );
  write_digit( time[2], digit1, digit2 );
  write_digit( time[1], digit2, digit3 );
  write_digit( time[0], digit3, digit4 );
}

void blankdisplay( unsigned int blanktime_microsec )
{
  digitalWrite(digit1, LOW);
  digitalWrite(digit2, LOW);
  digitalWrite(digit3, LOW);
  digitalWrite(digit4, LOW);
  delayMicroseconds(blanktime_microsec);
}

void ActuateBrightness(unsigned char brightness_level_toset)
{
  if ( brightness_level_toset == 0 )
  {
    brightness_on_time = 150;
    brightness_off_time = 2850;
  } 
  if ( brightness_level_toset == 1 )
  {
    brightness_on_time = 250;
    brightness_off_time = 2750;
  } 
  if ( brightness_level_toset == 2 )
  {
    brightness_on_time = 500;
    brightness_off_time = 2500;
  } 
  if ( brightness_level_toset == 3 )
  {
    brightness_on_time = 1000;
    brightness_off_time = 2000;
  }     
    if ( brightness_level_toset == 4 )
  {
    brightness_on_time = 1750;
    brightness_off_time = 1250;
  }     
    if ( brightness_level_toset == 5 )
  {
    brightness_on_time = 2500;
    brightness_off_time = 500;
  }
    if ( brightness_level_toset == 6 )
  {
    brightness_on_time = 2999;   // Never call delayMicroseconds with argument 0, exhibits strange behaviour.
    brightness_off_time = 1;
  }       
}

