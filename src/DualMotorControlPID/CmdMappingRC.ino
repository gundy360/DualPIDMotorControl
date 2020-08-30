void CmdCalcRC(){
  
  //Left Control
  if (LeftCmdBilinear > 0) { //Forward
    if(LeftState != FWD){
      LeftState = FWD;
      LeftPID.SetMode(MANUAL); //Reset PID
      OutputLeftPID = 0;
      LeftPID.SetMode(AUTOMATIC);
    }
    FreqSetpointLeft = map(LeftCmdBilinear, 0, CMD_MAX, CMD_FREQ_MIN, CMD_FREQ_MAX);
    FreqSetpointLeft = min((double)CMD_FREQ_MAX,FreqSetpointLeft);
  }else if (LeftCmdBilinear < 0) { //Reverse
    if(LeftState != REV){
      LeftState = REV;
      LeftPID.SetMode(MANUAL); //Reset PID
      OutputLeftPID = 0;
      LeftPID.SetMode(AUTOMATIC);
    } 
    FreqSetpointLeft = map(LeftCmdBilinear, 0, -CMD_MAX, -CMD_FREQ_MIN, -CMD_FREQ_MAX);
    FreqSetpointLeft = max((double)-CMD_FREQ_MAX, FreqSetpointLeft);
  } else { //Stopped
    FreqSetpointLeft = 0;
    LeftState = STOP;
    LeftPID.SetMode(MANUAL);
    OutputLeftPID = 0;
  }
  AbsFreqSetpointLeft = abs(FreqSetpointLeft);

  //Right Control
  if (RightCmdBilinear > 0) { //Forward
    if(RightState != FWD){
      RightState = FWD;
      RightPID.SetMode(MANUAL);
      OutputRightPID = 0; //Reset PID
      RightPID.SetMode(AUTOMATIC);
    }
    FreqSetpointRight = map(RightCmdBilinear, 0, CMD_MAX, CMD_FREQ_MIN, CMD_FREQ_MAX);
    FreqSetpointRight = min((double)CMD_FREQ_MAX, FreqSetpointRight);
  }else if (RightCmdBilinear < 0) { //Reverse
    if(RightState != REV){
      RightState = REV;
      RightPID.SetMode(MANUAL);
      OutputRightPID = 0; //Reset PID
      RightPID.SetMode(AUTOMATIC);
    } 
    FreqSetpointRight = map(RightCmdBilinear, 0, -CMD_MAX, -CMD_FREQ_MIN, -CMD_FREQ_MAX);
    FreqSetpointRight = max((double)-CMD_FREQ_MAX, FreqSetpointRight);
  } else { //Stopped
    FreqSetpointRight = 0;
    RightState = STOP;
    RightPID.SetMode(MANUAL);
    OutputRightPID = 0;
  }
  AbsFreqSetpointRight = abs(FreqSetpointRight);
}


//*******^Y^*******
//Q22**Q12|Q12**Q22
//********|********
//Q21**Q11|Q11**Q21
//--------+--------X->
//Q21**Q11|Q11**Q21
//********|********
//Q22**Q12|Q12**Q22

// channel_1 is FWD/REV
// channel_2 is RIGHT/LEFT

int BilinearInterpolate(int XIn, int YIn, int Q11, int Q12, int Q21, int Q22){ //X and Y assumed to be 0-10000
  int AtXZero, AtXMax, Output;
  AtXZero = map(YIn, 0, CMD_MAX, Q11, Q12);
  AtXMax = map(YIn, 0, CMD_MAX, Q21, Q22);
  Output = map(XIn, 0, CMD_MAX, AtXZero, AtXMax);
  return Output;  
}

void MixCmds(){
  if ((channel_1 <= 500) || (channel_1 >= 2500) || (channel_2 <= 500) || (channel_2 >= 2500)){ //Check for out of range RC receiver values
    LeftCmdBilinear = 0;
    RightCmdBilinear = 0;
  } else {
    channel_1 = min(channel_1, (int32_t)RC_POS_MAX);
    channel_2 = min(channel_2, (int32_t)RC_POS_MAX);
    channel_1 = max(channel_1, (int32_t)RC_NEG_MAX);
    channel_2 = max(channel_2, (int32_t)RC_NEG_MAX);
    if ((channel_1 >= RC_POS_MIN) && (channel_2 >= RC_POS_MIN)) { //Top-right quadrant
      
      FwdRevCmd = map(channel_1, RC_POS_MIN, RC_POS_MAX, 0, CMD_MAX);
      LeftRightCmd = map(channel_2, RC_POS_MIN, RC_POS_MAX, 0, CMD_MAX);
      LeftCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_FWD, CMD_COUNTERROTATE_MAX, CMD_CORNER_FWD_FAST);
      RightCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_FWD, -CMD_COUNTERROTATE_MAX, CMD_CORNER_FWD_SLOW);
      
    } else if ((channel_1 <= RC_NEG_MIN) && (channel_2 >= RC_POS_MIN)) { //Bottom-right quadrant
      
      FwdRevCmd = map(channel_1, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_MAX);
      LeftRightCmd = map(channel_2, RC_POS_MIN, RC_POS_MAX, 0, CMD_MAX);
      LeftCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_REV, CMD_COUNTERROTATE_MAX, CMD_CORNER_REV_FAST);
      RightCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_REV, -CMD_COUNTERROTATE_MAX, CMD_CORNER_REV_SLOW);
      
    } else if ((channel_1 <= RC_NEG_MIN) && (channel_2 <= RC_NEG_MIN)) { //Bottom-left quadrant
      
      FwdRevCmd = map(channel_1, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_MAX);
      LeftRightCmd = map(channel_2, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_MAX);
      LeftCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_REV, -CMD_COUNTERROTATE_MAX, CMD_CORNER_REV_SLOW);
      RightCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_REV, CMD_COUNTERROTATE_MAX, CMD_CORNER_REV_FAST);
      
    } else if ((channel_1 >= RC_POS_MIN) && (channel_2 <= RC_NEG_MIN)) { //Top-left quadrant
      
      FwdRevCmd = map(channel_1, RC_POS_MIN, RC_POS_MAX, 0, CMD_MAX);
      LeftRightCmd = map(channel_2, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_MAX);
      LeftCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_FWD, -CMD_COUNTERROTATE_MAX, CMD_CORNER_FWD_SLOW);
      RightCmdBilinear = BilinearInterpolate(LeftRightCmd, FwdRevCmd, 0, CMD_MAX_FWD, CMD_COUNTERROTATE_MAX, CMD_CORNER_FWD_FAST);
      
    } else if (channel_1 >= RC_POS_MIN) { //FWD
      
      LeftCmdBilinear = map(channel_1, RC_POS_MIN, RC_POS_MAX, 0, CMD_MAX_FWD);
      RightCmdBilinear = LeftCmdBilinear;
      
    } else if (channel_1 <= RC_NEG_MIN) { //REV
      
      LeftCmdBilinear = map(channel_1, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_MAX_REV);
      RightCmdBilinear = LeftCmdBilinear;
      
    } else if (channel_2 >= RC_POS_MIN) { //Right Countersteer
      
      LeftCmdBilinear = map(channel_2, RC_POS_MIN, RC_POS_MAX, 0, CMD_COUNTERROTATE_MAX);
      RightCmdBilinear = map(channel_2, RC_POS_MIN, RC_POS_MAX, 0, -CMD_COUNTERROTATE_MAX);
      
    } else if (channel_2 <= RC_NEG_MIN) { //Left Countersteer
      
      LeftCmdBilinear = map(channel_2, RC_NEG_MIN, RC_NEG_MAX, 0, -CMD_COUNTERROTATE_MAX);
      RightCmdBilinear = map(channel_2, RC_NEG_MIN, RC_NEG_MAX, 0, CMD_COUNTERROTATE_MAX);
      
    } else { //Center Deadzone Region
      
      LeftCmdBilinear = 0;
      RightCmdBilinear = 0;
      
    }
  }
}
