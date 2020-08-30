void UpdateFreqEncoderLeft(){

  //enc.value() goes between 0 and PPR.  It overflows and increments revolutionsLeft
  CurrentCountEncoderLeft = encLeft.value()+revolutionsLeft*PPR;

  //detect and handle millis rollover.  Slight update delay if this happens.
  if (millis() < PreviousTimeLeft){
    PreviousTimeLeft = millis();
    PreviousCountEncoderLeft = CurrentCountEncoderLeft;
  }
  
  //Detect Count Overflow.  Assumes encoder isn't going at ludacris speed.  Resets everything.  Slightly delays frequency update when detected. revolutions should never overflow/underflow
  if ((CurrentCountEncoderLeft < -ONE_BILLION) && (PreviousCountEncoderLeft > ONE_BILLION)){
    PreviousTimeLeft = millis();
    revolutionsLeft = 0;
    encLeft.reset();
    PreviousCountEncoderLeft = 0;
    CurrentCountEncoderLeft = 0;
  } else if ((PreviousCountEncoderLeft < -ONE_BILLION) && (CurrentCountEncoderLeft > ONE_BILLION)){ //Detect Count Underflow.  Reset like overflow
    PreviousTimeLeft = millis();
    revolutionsLeft = 0;
    encLeft.reset();
    PreviousCountEncoderLeft = 0;
    CurrentCountEncoderLeft = 0;
  }
  
  //Calculate Frequency in Hz
  if (millis() - PreviousTimeLeft >= FREQ_INTERVAL){
    long DeltaCount = 0;
    long DeltaTime = 0;
    DeltaCount = CurrentCountEncoderLeft - PreviousCountEncoderLeft;
    DeltaTime = millis() - PreviousTimeLeft;
    EncoderLeftFreq = (DeltaCount*1000)/DeltaTime;
    FreqMeasuredLeft = (double(abs(DeltaCount))*1000)/double(DeltaTime);
    PreviousCountEncoderLeft = CurrentCountEncoderLeft;
    
    PreviousTimeLeft = millis();
  }
}

void UpdateFreqEncoderRight(){

  //enc.value() goes between 0 and PPR.  It overflows and increments revolutionsRight  
  CurrentCountEncoderRight = encRight.value()+revolutionsRight*PPR;

  //detect and handle millis rollover.  Slight update delay if this happens.
  if (millis() < PreviousTimeRight){
    PreviousTimeRight = millis();
    PreviousCountEncoderRight = CurrentCountEncoderRight;
  }
  
  //Detect Count Overflow.  Assumes encoder isn't going at ludacris speed.  Resets everything.  Slightly delays frequency update when detected. revolutions should never overflow/underflow
  if ((CurrentCountEncoderRight < -ONE_BILLION) && (PreviousCountEncoderRight > ONE_BILLION)){
    PreviousTimeRight = millis();
    revolutionsRight = 0;
    encRight.reset();
    PreviousCountEncoderRight = 0;
    CurrentCountEncoderRight = 0;
  } else if ((PreviousCountEncoderRight < -ONE_BILLION) && (CurrentCountEncoderRight > ONE_BILLION)){ //Detect Count Underflow.  Reset like overflow
    PreviousTimeRight = millis();
    revolutionsRight = 0;
    encRight.reset();
    PreviousCountEncoderRight = 0;
    CurrentCountEncoderRight = 0;
  }
  
  //Calculate Frequency in Hz
  if (millis() - PreviousTimeRight >= FREQ_INTERVAL){
    long DeltaCount = 0;
    long DeltaTime = 0;
    DeltaCount = CurrentCountEncoderRight - PreviousCountEncoderRight;
    DeltaTime = millis() - PreviousTimeRight;
    EncoderRightFreq = (DeltaCount*1000)/DeltaTime;
    FreqMeasuredRight = (double(abs(DeltaCount))*1000)/double(DeltaTime);
    PreviousCountEncoderRight = CurrentCountEncoderRight;
    
    PreviousTimeRight = millis();
  }
}

/*interrupt handler. 
  this will called every time the counter goes through zero 
  and increment/decrement the variable depending on the direction of the encoder. 
*/
void funcLeft() {  
  if (encLeft.getDirection() == POSITIVE) revolutionsLeft++;
  if (encLeft.getDirection() == NEGATIVE) revolutionsLeft--;
}

void funcRight() {  
  if (encRight.getDirection() == POSITIVE) revolutionsRight++;
  if (encRight.getDirection() == NEGATIVE) revolutionsRight--;
}
