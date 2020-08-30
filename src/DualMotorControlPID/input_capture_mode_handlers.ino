void handler_channel_1(void) {                           //This function is called when channel 1 is captured.
  if (0b1 & GPIOB_BASE->IDR  >> 6) {                     //If the receiver channel 1 input pulse on A0 is high.
    channel_1_start = TIMER4_BASE->CCR1;                 //Record the start time of the pulse.
    TIMER4_BASE->CCER |= TIMER_CCER_CC1P;                //Change the input capture mode to the falling edge of the pulse.
  }
  else {                                                 //If the receiver channel 1 input pulse on A0 is low.
    channel_1 = TIMER4_BASE->CCR1 - channel_1_start;     //Calculate the total pulse time.
    if (channel_1 < 0)channel_1 += 0xFFFF;               //If the timer has rolled over a correction is needed.
    TIMER4_BASE->CCER &= ~TIMER_CCER_CC1P;               //Change the input capture mode to the rising edge of the pulse.
  }
}

void handler_channel_2(void) {                           //This function is called when channel 2 is captured.
  if (0b1 & GPIOB_BASE->IDR >> 7) {                      //If the receiver channel 2 input pulse on A1 is high.
    channel_2_start = TIMER4_BASE->CCR2;                 //Record the start time of the pulse.
    TIMER4_BASE->CCER |= TIMER_CCER_CC2P;                //Change the input capture mode to the falling edge of the pulse.
  }
  else {                                                 //If the receiver channel 2 input pulse on A1 is low.
    channel_2 = TIMER4_BASE->CCR2 - channel_2_start;     //Calculate the total pulse time.
    if (channel_2 < 0)channel_2 += 0xFFFF;               //If the timer has rolled over a correction is needed.
    TIMER4_BASE->CCER &= ~TIMER_CCER_CC2P;               //Change the input capture mode to the rising edge of the pulse.
  }
}

void handler_channel_3(void) {                           //This function is called when channel 3 is captured.
  if (0b1 & GPIOB_BASE->IDR >> 8) {                      //If the receiver channel 3 input pulse on A2 is high.
    channel_3_start = TIMER4_BASE->CCR3;                 //Record the start time of the pulse.
    TIMER4_BASE->CCER |= TIMER_CCER_CC3P;                //Change the input capture mode to the falling edge of the pulse.
  }
  else {                                                 //If the receiver channel 3 input pulse on A2 is low.
    channel_3 = TIMER4_BASE->CCR3 - channel_3_start;     //Calculate the total pulse time.
    if (channel_3 < 0)channel_3 += 0xFFFF;               //If the timer has rolled over a correction is needed.
    TIMER4_BASE->CCER &= ~TIMER_CCER_CC3P;               //Change the input capture mode to the rising edge of the pulse.
  }
}

void handler_channel_4(void) {                           //This function is called when channel 4 is captured.
  if (0b1 & GPIOB_BASE->IDR >> 9) {                      //If the receiver channel 4 input pulse on A3 is high.
    channel_4_start = TIMER4_BASE->CCR4;                 //Record the start time of the pulse.
    TIMER4_BASE->CCER |= TIMER_CCER_CC4P;                //Change the input capture mode to the falling edge of the pulse.
  }
  else {                                                 //If the receiver channel 4 input pulse on A3 is low.
    channel_4 = TIMER4_BASE->CCR4 - channel_4_start;     //Calculate the total pulse time.
    if (channel_4 < 0)channel_4 += 0xFFFF;               //If the timer has rolled over a correction is needed.
    TIMER4_BASE->CCER &= ~TIMER_CCER_CC4P;               //Change the input capture mode to the rising edge of the pulse.
  }
}
