///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setup for timers reading receiver pulses
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TIMER1
////Channel 1 - PA8 - Left PWM to motor controller
////Channel 2 - PA9 - Serial
////Channel 3 - PA10 - Serial
////Channel 4 - PA11 - Right PWM to motor controller
//TIMER2
////Channel 1 - PA0 - Right Encoder Channel 1
////Channel 2 - PA1 - Right Encoder Channel 2
////Channel 3 - PA2 - Open - no high resolustion pwm
////Channel 4 - PA3 - Open - no high resolustion pwm
//TIMER3
////Channel 1 - PA6 - Left Encoder Channel 1
////Channel 2 - PA7 - Left Encoder Channel 2
////Channel 3 - PB0 - Open - no high resolustion pwm
////Channel 4 - PB1 - Open - no high resolustion pwm
//TIMER4
////Channel 1 - PB6 - RC PPM Channel_1
////Channel 2 - PB7 - RC PPM Channel_2
////Channel 3 - PB8 - RC PPM Channel_3 - Unused
////Channel 4 - PB9 - RC PPM Channel_4 - Unused
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timer_setup_PPM(void) {
  Timer4.attachCompare1Interrupt(handler_channel_1);
  Timer4.attachCompare2Interrupt(handler_channel_2);
  Timer4.attachCompare3Interrupt(handler_channel_3);
  Timer4.attachCompare4Interrupt(handler_channel_4);
  TIMER4_BASE->CR1 = TIMER_CR1_CEN;
  TIMER4_BASE->CR2 = 0;
  TIMER4_BASE->SMCR = 0;
  TIMER4_BASE->DIER = TIMER_DIER_CC1IE | TIMER_DIER_CC2IE | TIMER_DIER_CC3IE | TIMER_DIER_CC4IE;
  TIMER4_BASE->EGR = 0;
  TIMER4_BASE->CCMR1 = 0b100000001; //Register is set like this due to a bug in the define table (30-09-2017)
  TIMER4_BASE->CCMR2 = 0b100000001; //Register is set like this due to a bug in the define table (30-09-2017)
  TIMER4_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC2E | TIMER_CCER_CC3E | TIMER_CCER_CC4E;
  TIMER4_BASE->PSC = 71;
  TIMER4_BASE->ARR = 0xFFFF;
  TIMER4_BASE->DCR = 0;
}
