void tick()
{
  time[0]++;
  if ( time[0] == 10 ) { time[0] = 0; time[1]++; }
  if ( time[1] == 6 )  { time[1] = 0; time[2]++; }
  if ( time[2] == 10 ) { time[2] = 0; time[3]++; }
  if ( time[3] == 6 )  { time[0] = 0; time[1] = 0; time[2] = 0; time[3] = 0; } 
    /// Need to renable interupts as I2C needs them.
  // Green time as been reached, switch on green light.
  if ( time[0] == greentime[0] && time[1] == greentime[1] && time[2] == greentime[2] && time[3] == greentime[3] )
  {
    sei(); 
    LED_TenMM(GREEN_BLINK);
    if ( currentmode == 0 ) { LED_ThreeMM(GREEN); }
    ten_mm_ledstate++;
  } 
  if ( time[0] == yellowtime[0] && time[1] == yellowtime[1] && time[2] == yellowtime[2] && time[3] == yellowtime[3] )
  {
    sei(); 
    LED_TenMM(YELLOW_BLINK);
    if ( currentmode == 0 ) { LED_ThreeMM(YELLOW); }
    ten_mm_ledstate++;
  }   
    if ( time[0] == redtime[0] && time[1] == redtime[1] && time[2] == redtime[2] && time[3] == redtime[3] )
  {
    sei(); 
    LED_TenMM(RED_BLINK);
    if ( currentmode == 0 ) { LED_ThreeMM(RED); }
    ten_mm_ledstate++;
  } 
}

void preset1act()
{
    auto unsigned char presetheld = 0; 
    while (digitalRead(preset1) == LOW )
    {
          delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383);
          presetheld++;
          if ( presetheld >= 31 )
          {
            ModifyPresetAct(preset1times);
          }
    }  
    PresetAct(preset1times);    
    CheckAfterLightTimeChange();
}    

void preset2act()
{
    auto unsigned char presetheld = 0;
    while (digitalRead(preset2) == LOW )
    {
          delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383);
          presetheld++;
          if ( presetheld >= 31 )
          {
            ModifyPresetAct(preset2times);
          }
    } 
    PresetAct(preset2times);
    CheckAfterLightTimeChange();
}    

void preset3act()
{
    auto unsigned char presetheld = 0;
    while ( analogRead (7) < 0xFF ) 
    {
       delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383);
       presetheld++;
       if ( presetheld >= 31 )
       {
            ModifyPresetAct(preset3times);
       }
    }   
    PresetAct(preset3times);
    CheckAfterLightTimeChange();
}    

void preset4act()
{
    auto unsigned char presetheld = 0;
    while ( analogRead (6) < 0xFF ) 
    {
       delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383); delayMicroseconds(16383);
       presetheld++;
       if ( presetheld >= 31 )
       {
            ModifyPresetAct(preset4times);
       }
    }
    PresetAct(preset4times);   
    CheckAfterLightTimeChange();
}    

// Cycle between modes 0 and 1. Implement 2, 3 and 4 later.
void modeselect()
{
  auto unsigned int timeinseconds = 0;
  if ( currentmode == 0 ) 
  { 
    sei(); 
    LED_ThreeMM(GREEN_BLINK); 
    currentmode = 1;  
  }  
  else if ( currentmode == 1 ) 
  {   
    timeinseconds = ResolveTimeinSeconds(greentime);
    p("Green Time is now in seconds: %d\n", timeinseconds);
    if ( timeinseconds >= ResolveTimeinSeconds(yellowtime) )
    {    
      PushTimeinSeconds(timeinseconds+5,yellowtime);
      PushTimeinSeconds(timeinseconds+10,redtime);
    }  
    CheckAfterLightTimeChange();
    sei(); LED_ThreeMM(YELLOW_BLINK);  
    currentmode = 2; 
  }
  else if ( currentmode == 2 ) 
  {
    timeinseconds = ResolveTimeinSeconds(yellowtime);
    p("Yellow Time is now in seconds: %d\n", timeinseconds);
    if ( timeinseconds >= ResolveTimeinSeconds(redtime) )
    {    
      PushTimeinSeconds(timeinseconds+5,redtime);
    }    
    CheckAfterLightTimeChange();
    sei(); LED_ThreeMM(RED_BLINK); 
    currentmode = 3;
  }
 
  else if ( currentmode == 3 )
  { 
    // Going back to normal stopwatch mode. Return 3mm led state to match 10mm led state
    if (ten_mm_ledstate == 1) {   sei(); LED_ThreeMM(GREEN); }  // Turn the 3mm green back on to match 10mm one    
    else if (ten_mm_ledstate == 2) { sei();  LED_ThreeMM(YELLOW); }  // Turn the 3mm yellow back on to match 10mm one
    else if (ten_mm_ledstate == 3) { sei();  LED_ThreeMM(RED); }  // Turn the 3mm red back on to match 10mm one
    else { sei(); LED_ThreeMM(ALLOFF); }
    CheckAfterLightTimeChange(); 
    currentmode = 0; 
  }  

  else if ( currentmode == 4)    // Current mode is 4 (stored times), only a long press of mode button gets here. Going to brightness set mode
  { 
    // The display function for mode 4 has already turned 3mm led off, so nothing to do here, they stay off for mode 5
    currentmode = 5; 
  } 
  
  else  // Current mode must be 5 (brightness set) to get here. Going back to normal stopwatch mode.  Restore 3mm LEDs to match 10mm state
  { 
    if (ten_mm_ledstate == 1) {   sei(); LED_ThreeMM(GREEN); }  // Turn the 3mm green back on to match 10mm one 
    else if (ten_mm_ledstate == 2) { sei();  LED_ThreeMM(YELLOW); }  // Turn the 3mm yellow back on to match 10mm one
    else if (ten_mm_ledstate == 3) { sei();  LED_ThreeMM(RED); }  // Turn the 3mm red back on to match 10mm one
    else { sei(); LED_ThreeMM(ALLOFF); } 
    currentmode = 0; 
  }    
}

// Plus button pushed.
void plusact()
{
   if ( currentmode == 0 )
   {
    if ( digitalRead(minusbut) == LOW )   // If both buttons are pressed
    {
      SaveTimeToStore();
      time[3] = 0; time[2] = 0; time[1] = 0; time[0] = 0;    // reset the stopwatch
      sei();         // Enable interupts again as writing to LEDS needs them
      Timer1.stop();
      LED_AllOff();   // Turn all LED off again.
      ten_mm_ledstate=0;
    }
    else
    {
      Timer1.stop();
    }  
  }
  if ( currentmode == 1 )
  {
    LightTimeIncrease(greentime);
  }
    if ( currentmode == 2 )
  {
   LightTimeIncrease(yellowtime);
  }
    if ( currentmode == 3 )
  {
   LightTimeIncrease(redtime);
  }
  if ( currentmode == 4 )
  {
    if ( storeindex_display != 0 ) { storeindex_display--; }   
  }
  if ( currentmode == 5 )
  {
    if ( brightness_level < 6  ) { brightness_level++; }
    ActuateBrightness(brightness_level);   
  }  
  
} 

// Minus button pushed. Make it stall at 00:00 instead of wrapping round.
void minusact()
{
   if ( currentmode == 0 )
  {
    if ( digitalRead(plusbut) == LOW )
    {
      SaveTimeToStore();
      time[3] = 0; time[2] = 0; time[1] = 0; time[0] = 0;    // reset the stopwatch
      sei();         // Enable interupts again as writing to LEDS needs them  
      LED_AllOff();   // Turn all LED off again
      Timer1.stop();
      ten_mm_ledstate=0; 
    }
    else
    {
      Timer1.resume();
    }  
  }

  if ( currentmode == 1 &&  ( greentime[0] > 0 || greentime[1] > 0 || greentime[2] > 0 || greentime[3] > 0 ) )
   {
      LightTimeDecrease(greentime);
   }
  if ( currentmode == 2 &&  ( yellowtime[0] > 0 || yellowtime[1] > 0 || yellowtime[2] > 0 || yellowtime[3] > 0 ) )
   {
      LightTimeDecrease(yellowtime);
   }
  if ( currentmode == 3 &&  ( redtime[0] > 0 || redtime[1] > 0 || redtime[2] > 0 || redtime[3] > 0 ) )
   {
      LightTimeDecrease(redtime);
   }
  if ( currentmode == 4 )
  {
    if ( storeindex_display != stored_count - 1 ) { storeindex_display++; }
  }
    if ( currentmode == 5 )
  {
    if ( brightness_level > 0  ) { brightness_level--; }
   ActuateBrightness(brightness_level);   
  }  
  
  
}  


