//PCA9552 LED driver on i2c bus at address 0x67.

void LED_AllOff()   // Turn all LED off
{
  Wire.begin();
  Wire.beginTransmission(0x67);
  Wire.write(0x16);
  Wire.write(0x55);
  Wire.write(0x55);  
  Wire.write(0x55);
  Wire.endTransmission();
}

void LED_SetupBlink()   // Setup Blink Rates for 3mm LEDs
{
  Wire.begin();
  Wire.beginTransmission(0x67); 
  Wire.write(0x12);
  Wire.write(0xFF);   // Set period 0 of LED blink to maximum
  Wire.write(0x04);   // Set Duty Cycle of PWM 0. Higher number more time off.
  Wire.write(0x0C);   // Set period 1 of LED blink to quarter of max
  Wire.endTransmission();
}  
  

void LED_TenMM(unsigned char state)
{
  Wire.begin();
  Wire.beginTransmission(0x67);
  Wire.write(0x08);
  
  if ( state == 0 ) { Wire.write(0x55); }
  else if ( state == 1 ) { Wire.write(0x54); }
  else if ( state == 2 ) { Wire.write(0x56); }
  else if ( state == 3 ) { Wire.write(0x45); }
  else if ( state == 4 ) { Wire.write(0x65); }
  else if ( state == 5 ) { Wire.write(0x51); }
  else if ( state == 6 ) { Wire.write(0x59); }  
  Wire.endTransmission();
}

void LED_ThreeMM(unsigned char state)
{
  Wire.begin();
  Wire.beginTransmission(0x67);
  Wire.write(0x06);
  
  if ( state == 0 ) { Wire.write(0x55); }
  else if ( state == 1 ) { Wire.write(0x54); }
  else if ( state == 2 ) { Wire.write(0x57); }
  else if ( state == 3 ) { Wire.write(0x45); }
  else if ( state == 4 ) { Wire.write(0x75); }
  else if ( state == 5 ) { Wire.write(0x51); }
  else if ( state == 6 ) { Wire.write(0x5D); }  
  Wire.endTransmission();
}


