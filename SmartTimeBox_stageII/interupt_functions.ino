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
    preset_ack(1);  
    if (digitalRead(preset1) == LOW && !saw_but_release) // If button still held down after displaying ps1, then user wants to reprogram the preset with current times
    {
            ModifyPresetAct(but_preset_times[0].preset_times);
            return;

    }  
    if (!preset_just_pressed) {  
     current_presettime=0;
     last_preset_time = millis();
     preset_just_pressed = 1;
    }
    else {
      current_presettime=4;
    }      

    PresetAct(but_preset_times[current_presettime].preset_times);
    CheckAfterLightTimeChange();
}    

void preset2act()
{
    preset_ack(2);
    if (digitalRead(preset2) == LOW && !saw_but_release)
    {
            ModifyPresetAct(but_preset_times[1].preset_times);
            return;
    } 
    if (!preset_just_pressed) {  
     current_presettime=1;
     last_preset_time = millis();
     preset_just_pressed = 1;
    }
    else {
      current_presettime=5;
    }      

    PresetAct(but_preset_times[current_presettime].preset_times);
    CheckAfterLightTimeChange();
}    

void preset3act()
{
    preset_ack(3);
    if ( analogRead (7) < 0xF0  && !saw_but_release) 
    {
      ModifyPresetAct(but_preset_times[2].preset_times);
      return;
    }
    if (!preset_just_pressed) {  
      current_presettime=2;
      last_preset_time = millis();
      preset_just_pressed = 1;
    }
    else {
      current_presettime=6;
    }       
    PresetAct(but_preset_times[current_presettime].preset_times);
    CheckAfterLightTimeChange();
}    

void preset4act()
{
    preset_ack(4);
    if ( analogRead (6) < 0xF0 && !saw_but_release) 
    {
      ModifyPresetAct(but_preset_times[3].preset_times);
      return;
    }
    if (!preset_just_pressed) {  
      current_presettime=3;
      last_preset_time = millis();
      preset_just_pressed = 1;
    }
    else {
      current_presettime=7;
    }    
    PresetAct(but_preset_times[current_presettime].preset_times); 
    CheckAfterLightTimeChange();
}    

// Cycle between modes 0 and 1. Implement 2, 3 and 4 later.
void modeselect()
{
  auto unsigned int timeinseconds = 0;
  auto unsigned int yellowlessgreentimeinseconds = 0;
  if ( currentmode == 0 ) 
  { 
    sei(); 
    LED_ThreeMM(GREEN_BLINK); 
    currentmode = 1;  
  }
  else if ( currentmode == 1 ) 
  { 
    sei(); 
    LED_ThreeMM(YELLOW_BLINK); 
    currentmode = 2;  
  }  
  else if ( currentmode == 2 ) 
  { 
    timeinseconds = ResolveTimeinSeconds(yellowtime);
    // p("Yellow Time is now in seconds: %d\n", timeinseconds);
    if ( timeinseconds >= ResolveTimeinSeconds(redtime) )
    {    
      yellowlessgreentimeinseconds=ResolveTimeinSeconds(yellowtime)-ResolveTimeinSeconds(greentime);
      PushTimeinSeconds(timeinseconds+yellowlessgreentimeinseconds,redtime);
    }    
    CheckAfterLightTimeChange();
    sei(); 
    LED_ThreeMM(RED_BLINK); 
    currentmode = 3;  
  }
  else if ( currentmode == 3 ) 
  { 
    sei(); 
    LED_ThreeMM(ALLOFF); 
    currentmode = 0;  
  }   
  else if ( currentmode == 4 ) 
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
    currentmode = 5; 
  }
  else if ( currentmode == 5 ) 
  {
    timeinseconds = ResolveTimeinSeconds(yellowtime);
    p("Yellow Time is now in seconds: %d\n", timeinseconds);
    if ( timeinseconds >= ResolveTimeinSeconds(redtime) )
    {    
      PushTimeinSeconds(timeinseconds+5,redtime);
    }    
    CheckAfterLightTimeChange();
    sei(); LED_ThreeMM(RED_BLINK); 
    currentmode = 6;
  }
 
  else if ( currentmode == 6 )
  { 
    // Going back to normal stopwatch mode. Return 3mm led state to match 10mm led state
    if (ten_mm_ledstate == 1) {   sei(); LED_ThreeMM(GREEN); }  // Turn the 3mm green back on to match 10mm one    
    else if (ten_mm_ledstate == 2) { sei();  LED_ThreeMM(YELLOW); }  // Turn the 3mm yellow back on to match 10mm one
    else if (ten_mm_ledstate == 3) { sei();  LED_ThreeMM(RED); }  // Turn the 3mm red back on to match 10mm one
    else { sei(); LED_ThreeMM(ALLOFF); }
    CheckAfterLightTimeChange(); 
    currentmode = 0; 
  }  

  else if ( currentmode == 7)    // Current mode is 7 (stored times), only a second long press of mode button gets here. Going to brightness set mode
  { 
    // The display function for mode 7 has already turned 3mm led off, so nothing to do here, they stay off for mode 8
    currentmode = 8; 
  } 
  
  else  // Current mode must be 8 (brightness set) to get here. Going back to normal stopwatch mode.  Restore 3mm LEDs to match 10mm state
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
    if ( current_standardtime == 19 ) { current_standardtime == 0; }
    else { current_standardtime++; }
    PresetAct(standard_times[current_standardtime].preset_times);
    CheckAfterLightTimeChange();
  }  
    if ( currentmode == 2 )
  {
    auto char n=0;
    for (n=0;n<60;n++)  { LightTimeIncrease(yellowtime); }   // Just call the increase by 1 sec funciton 60 times instead of creating new function 
    CheckAfterLightTimeChange();
  }
    if ( currentmode == 3 )
  {
    auto char n=0;
    for (n=0;n<60;n++)  { LightTimeIncrease(redtime); }   // Just call the increase by 1 sec funciton 60 times instead of creating new function 
    CheckAfterLightTimeChange();
  }  
  
  if ( currentmode == 4 )
  {
    LightTimeIncrease(greentime);
  }
    if ( currentmode == 5 )
  {
   LightTimeIncrease(yellowtime);
  }
    if ( currentmode == 6 )
  {
   LightTimeIncrease(redtime);
  }
  if ( currentmode == 7 )
  {
    if ( storeindex_display != 0 ) { storeindex_display--; }   
  }
  if ( currentmode == 8 )
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
  if ( currentmode == 1 )
  {
    if ( current_standardtime == 0 ) { current_standardtime == 19; }
    else { current_standardtime--; }
    PresetAct(standard_times[current_standardtime].preset_times);
    CheckAfterLightTimeChange();
  }  
  if ( currentmode == 2 )
  {
    auto char n=0;
    for (n=0;n<60;n++)  { LightTimeDecrease(yellowtime); }   // Just call the decrease by 1 sec funciton 60 times instead of creating new function 
    CheckAfterLightTimeChange();
  }
  if ( currentmode == 3 )
  {
    auto char n=0;
    for (n=0;n<60;n++)  { LightTimeDecrease(redtime); }   // Just call the decrease by 1 sec funciton 60 times instead of creating new function 
    CheckAfterLightTimeChange();
  }  
  
  if ( currentmode == 4 &&  ( greentime[0] > 0 || greentime[1] > 0 || greentime[2] > 0 || greentime[3] > 0 ) )
  {
      LightTimeDecrease(greentime);
  }
  if ( currentmode == 5 &&  ( yellowtime[0] > 0 || yellowtime[1] > 0 || yellowtime[2] > 0 || yellowtime[3] > 0 ) )
  {
       LightTimeDecrease(yellowtime);
  }
  if ( currentmode == 6 &&  ( redtime[0] > 0 || redtime[1] > 0 || redtime[2] > 0 || redtime[3] > 0 ) )
   {
      LightTimeDecrease(redtime);
   }
  if ( currentmode == 7 )
  {
    if ( storeindex_display != stored_count - 1 ) { storeindex_display++; }
  }
    if ( currentmode == 8 )
  {
    if ( brightness_level > 0  ) { brightness_level--; }
   ActuateBrightness(brightness_level);   
  }  
  
  
}  


