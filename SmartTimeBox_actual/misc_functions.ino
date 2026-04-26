void PresetAct(struct timearray *presettimes)
{
     greentime[3] = presettimes[0].storedtime[3]; greentime[2] = presettimes[0].storedtime[2]; greentime[1] = presettimes[0].storedtime[1]; greentime[0] = presettimes[0].storedtime[0];
    yellowtime[3] = presettimes[1].storedtime[3]; yellowtime[2] = presettimes[1].storedtime[2]; yellowtime[1] = presettimes[1].storedtime[1]; yellowtime[0] = presettimes[1].storedtime[0];
    redtime[3] = presettimes[2].storedtime[3]; redtime[2] = presettimes[2].storedtime[2]; redtime[1] = presettimes[2].storedtime[1]; redtime[0] = presettimes[2].storedtime[0];
} 

void ModifyPresetAct(struct timearray *presettimes)
{
     presettimes[0].storedtime[3] = greentime[3]; presettimes[0].storedtime[2] = greentime[2]; presettimes[0].storedtime[1] = greentime[1]; presettimes[0].storedtime[0] = greentime[0];
     presettimes[1].storedtime[3] = yellowtime[3]; presettimes[1].storedtime[2] = yellowtime[2]; presettimes[1].storedtime[1] = yellowtime[1]; presettimes[1].storedtime[0] = yellowtime[0];
     presettimes[2].storedtime[3] = redtime[3]; presettimes[2].storedtime[2] = redtime[2]; presettimes[2].storedtime[1] = redtime[1]; presettimes[2].storedtime[0] = redtime[0];
} 


void SaveTimeToStore()
{
    auto unsigned char n = 0;
       for ( n = stored_count; n > 0;n-- ) // Save the time to stored time register before clearing the stopwatch
      {
        storedtimes[n] = storedtimes[n - 1];
      }  
      storedtimes[0].storedtime[3] = time[3]; storedtimes[0].storedtime[2] = time[2];
      storedtimes[0].storedtime[1] = time[1]; storedtimes[0].storedtime[0] = time[0];
      if ( stored_count < 63 ) { stored_count++; }
      storeindex_display = 0;
}   

void LightTimeIncrease( unsigned char *timecolour)
{
    timecolour[0]++; 
    if ( timecolour[0] == 10 ) { timecolour[0] = 0; timecolour[1]++; }
    if ( timecolour[1] == 6 )  { timecolour[1] = 0; timecolour[2]++; }
    if ( timecolour[2] == 10 ) { timecolour[2] = 0; timecolour[3]++; }
    if ( timecolour[3] == 6 )  { timecolour[0] = 0; timecolour[1] = 0; timecolour[2] = 0; timecolour[3] = 0; } 
} 

void DecrementLightTime(unsigned char *timecolour)
{
    timecolour[0]--;    //     *timecolour is unsigned, so will wrap around to 0xFF (255) if already 0 then decremented 
    if ( timecolour[0] == 255 ) { timecolour[0] = 9; timecolour[1]--; }
    if ( timecolour[1] == 255 )  { timecolour[1] = 5; timecolour[2]--; }
    if ( timecolour[2] == 255 ) { timecolour[2] = 9; timecolour[3]--; }
    if ( timecolour[3] == 255 )  { timecolour[3] = 0; timecolour[2] = 9; timecolour[1] = 5; timecolour[0] = 9; }
}

void LightTimeDecrease( unsigned char *timecolour)
{
   if ( currentmode == 1 )   { DecrementLightTime(timecolour); }     
   if ( currentmode == 2 && ResolveTimeinSeconds(yellowtime) > (ResolveTimeinSeconds(greentime)+1) )
   {
      DecrementLightTime(timecolour);
   }
   if ( currentmode == 3 && ResolveTimeinSeconds(redtime) > (ResolveTimeinSeconds(yellowtime)+1) )
   {
      DecrementLightTime(timecolour);
   }    
} 

void CheckAnalogSixandSeven()
{
        if (analogRead (7) < 0xFF ) { preset3act(); }
        if (analogRead (6) < 0xFF ) { preset4act(); }
}  

void TimeColourWrite(unsigned char *timecolour)
{

      if ( flash == 10 || flash == 20 || flash == 30 || flash == 40 ) 
      {
        CheckAnalogSixandSeven();
      } 
      if ( flash < 21 )
      {
	write_time(timecolour);
      }
     // Blank the other half of the cycle to create flashing effect.
      else
      {
         blankdisplay(13000);
      }   
    
 // Print out the button held down cycle count for debugging.
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

unsigned int ResolveTimeinSeconds( unsigned char *timecolour )
{
  auto unsigned int result = 0;
  result = timecolour[3]*600 + timecolour[2]*60 + timecolour[1]*10 + timecolour[0];
  return result;
} 

void PushTimeinSeconds( unsigned int time_seconds, unsigned char *timecolour )
{
  timecolour[3] = time_seconds/600;
  timecolour[2] = (time_seconds-(timecolour[3]*600))/60;
  timecolour[1] = (time_seconds-(timecolour[3]*600)-(timecolour[2]*60))/10;
  timecolour[0] = time_seconds-(timecolour[3]*600)-(timecolour[2]*60)-(timecolour[1]*10);
}  
  

void CheckAfterLightTimeChange(void)
{
  auto unsigned int current_time_seconds = 0;
  auto unsigned int greentime_seconds = 0;
  auto unsigned int yellowtime_seconds = 0;
  auto unsigned int redtime_seconds = 0;

  current_time_seconds = ResolveTimeinSeconds(time);    
  greentime_seconds = ResolveTimeinSeconds(greentime); 
  yellowtime_seconds = ResolveTimeinSeconds(yellowtime);
  redtime_seconds = ResolveTimeinSeconds(redtime);
  
  sei();
  if( current_time_seconds >= redtime_seconds ) { LED_TenMM(RED_BLINK); ten_mm_ledstate = 3; }
  else if( current_time_seconds >= yellowtime_seconds ) { LED_TenMM(YELLOW_BLINK); ten_mm_ledstate = 2; }
  else if( current_time_seconds >= greentime_seconds ) { LED_TenMM(GREEN_BLINK); ten_mm_ledstate = 1; }
  else { LED_TenMM(ALLOFF); ten_mm_ledstate = 0; }
  
  if ( currentmode == 0 )
  {
    if (ten_mm_ledstate == 1) {   sei(); LED_ThreeMM(GREEN); }  // Turn the 3mm green back on to match 10mm one    
    else if (ten_mm_ledstate == 2) { sei();  LED_ThreeMM(YELLOW); }  // Turn the 3mm yellow back on to match 10mm one
    else if (ten_mm_ledstate == 3) { sei();  LED_ThreeMM(RED); }  // Turn the 3mm red back on to match 10mm one
    else { sei(); LED_ThreeMM(ALLOFF); }
  }  
}  
  
  
  
