// Setup functions to write each number
void writezero() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, LOW);
}


void writeone() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writetwo() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, HIGH);
}

void writethree() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, HIGH);
}

void writefour() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writefive() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writesix() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writeseven() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);  
  digitalWrite(segG, LOW);
}

void writeeight() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void writenine() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);  
  digitalWrite(segG, HIGH);
}

void write_digit( unsigned char time_digit, int prev_digit, int curr_digit )
{
  digitalWrite(prev_digit, HIGH);
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
  digitalWrite(curr_digit, LOW);
  delayMicroseconds(3000);
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
  digitalWrite(digit1, HIGH);
  digitalWrite(digit2, HIGH);
  digitalWrite(digit3, HIGH);
  digitalWrite(digit4, HIGH);
  delayMicroseconds(blanktime_microsec);
}
