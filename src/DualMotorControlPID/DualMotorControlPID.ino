#include <STM32Encoder.h>
#include <PID_v1.h>

//Stick mixing settings (Bilinear Interpolation)
#define CMD_MAX 10000                           //Max Scaling value.  This value will command a frequency of CMD_FREQ_MAX
#define CMD_MAX_FWD CMD_MAX                     //Max Foward Speed
#define CMD_CORNER_FWD_FAST CMD_MAX_FWD         //When stick is in forward corner position, the speed of the outside wheel
#define CMD_CORNER_FWD_SLOW CMD_MAX_FWD/2       //When stick is in forward corner position, the speed of the inside wheel
#define CMD_MAX_REV -CMD_MAX*7/10               //Max Reverse Speed
#define CMD_CORNER_REV_FAST CMD_MAX_REV         //When stick is in reverse corner position, the speed of the outside wheel
#define CMD_CORNER_REV_SLOW CMD_MAX_REV/2       //When stick is in reverse corner position, the speed of the inside wheel
#define CMD_COUNTERROTATE_MAX CMD_MAX/3         //Max speed of both wheels when counter rotating

//Common encoder defines
#define FREQ_INTERVAL 10 //Calculate RPM using this big of window in ms
#define PPR 1200
#define ONE_BILLION 1000000000
#define CMD_FREQ_MIN 0
#define CMD_FREQ_MAX 30000  //Hz  //Frequency 12V Max Unloaded = 30,000.  Adjust to match your setup

//Motor Driver Defines.  Not using library since STM32 has better resolution nativily so this should have better control.
#define PWM_LEFT_PIN PA11
#define DIR_LEFT_PIN PA5
#define PWM_RIGHT_PIN PA8
#define DIR_RIGHT_PIN PA4
#define PWM_DUTY_MAX 65535
//PWM threshold for movement ~6000 unloaded 

//------------RC Input Variables---------------
int32_t channel_1_start, channel_1; //PB6
int32_t channel_2_start, channel_2; //PB7
int32_t channel_3_start, channel_3; //PB8
int32_t channel_4_start, channel_4; //PB9

int FwdRevCmd, LeftRightCmd;

//RC transmitter will likely have to be adjusted to match these ranges.  Not adjusting the transmitter will either not allow for full speed or will have deadzone at end of stick stroke.
#define RC_POS_MAX 2000
#define RC_POS_MIN 1520 //was 1550
#define RC_NEG_MAX 1000
#define RC_NEG_MIN 1480 //was 1450

int LeftState, RightState;

enum CMD_DIRECTION {
  STOP = 0,
  FWD = 1,
  REV = 2
};


//Passive Output Mapping
#define CAL_X_MIN 0
#define CAL_X_MAX CMD_FREQ_MAX //Frequency 12V Max Unloaded = 6,000
#define CAL_Y_MIN 6000 //PWM Threshold ~6000 PWM Command
#define CAL_Y_MAX PWM_DUTY_MAX

//Common PID Settings
#define PID_SAMPLE_TIME 10
#define PID_OUT_MAX PWM_DUTY_MAX/2

double CommonKp=0, CommonKi=4, CommonKd=0; //0.5,3,0

//--------------Left PID ------------------------
long LeftError;
int LeftCmdPassive, LeftCmdTotal, LeftCmdBilinear;
double AbsFreqSetpointLeft, FreqSetpointLeft, FreqMeasuredLeft, OutputLeftPID;
PID LeftPID(&FreqMeasuredLeft, &OutputLeftPID, &AbsFreqSetpointLeft, CommonKp, CommonKi, CommonKd, DIRECT); //Last is DIRECT or REVERSE (Output sign vs Error Sign)

//--------------Right PID ------------------------
long RightError;
int RightCmdPassive, RightCmdTotal, RightCmdBilinear;
double AbsFreqSetpointRight, FreqSetpointRight, FreqMeasuredRight, OutputRightPID;
PID RightPID(&FreqMeasuredRight, &OutputRightPID, &AbsFreqSetpointRight, CommonKp, CommonKi, CommonKd, DIRECT); //Last is DIRECT or REVERSE (Output sign vs Error Sign)

//--------------Left Encoder --------------------
//EncoderLeft Variables and Objects
volatile long revolutionsLeft = 0;//revolution counter
long CurrentCountEncoderLeft = 0;
long PreviousCountEncoderLeft = 0;
unsigned long PreviousTimeLeft = 0;
long EncoderLeftFreq = 0;
STM32Encoder encLeft(TIMER3, COUNT_BOTH_CHANNELS, 1, PPR);

//--------------Right Encoder --------------------
//EncoderLeft Variables and Objects
volatile long revolutionsRight = 0;//revolution counter
long CurrentCountEncoderRight = 0;
long PreviousCountEncoderRight = 0;
unsigned long PreviousTimeRight = 0;
long EncoderRightFreq = 0;
STM32Encoder encRight(TIMER2, COUNT_BOTH_CHANNELS, 1, PPR);

void setup() {

  timer_setup_PPM();                    //Setup the timers for the receiver inputs
  //configure inputs for the encoder
  //comment the timer not used. 
//  pinMode(PA8, INPUT_PULLUP);  //TIMER 1 channel 1 - TIMER 1 Used for PWM out, so this likely won't work as an optionS 
//  pinMode(PA9, INPUT_PULLUP);  //TIMER 1 channel 2  Also port for serial
  pinMode(PA0, INPUT_PULLUP);  //TIMER 2 channel 1 - Right Encoder 1
  pinMode(PA1, INPUT_PULLUP);  //TIMER 2 channel 2 - Right Encoder 2
  pinMode(PA6, INPUT_PULLUP);  //TIMER 3 channel 1 - Left Encoder 1
  pinMode(PA7, INPUT_PULLUP);  //TIMER 3 channel 2 - Left Encoder 2
//  pinMode(PB6, INPUT_PULLUP);  //TIMER 4 channel 1 TIMER 4 used for RC input, so this likely won't work as an option
//  pinMode(PB7, INPUT_PULLUP);  //TIMER 4 channel 2

  pinMode(PC13, OUTPUT);  //Board LED

  //Motor Control Pins
  pinMode(PWM_LEFT_PIN, PWM);
  pinMode(DIR_LEFT_PIN, OUTPUT);
  pinMode(PWM_RIGHT_PIN, PWM);
  pinMode(DIR_RIGHT_PIN, OUTPUT);

  //attach interrupt
  encLeft.attachInterrupt(funcLeft);//attach interrupt Left Encoder
  encRight.attachInterrupt(funcRight);//attach interrupt Right Encoder

  //PID Setup
  LeftPID.SetTunings(CommonKp, CommonKi, CommonKd);
  LeftPID.SetMode(AUTOMATIC); //Disabled is MANUAL
  LeftPID.SetOutputLimits(-PID_OUT_MAX, PID_OUT_MAX);
  LeftPID.SetSampleTime(PID_SAMPLE_TIME);

  RightPID.SetTunings(CommonKp, CommonKi, CommonKd);
  RightPID.SetMode(AUTOMATIC);
  RightPID.SetOutputLimits(-PID_OUT_MAX, PID_OUT_MAX);
  RightPID.SetSampleTime(PID_SAMPLE_TIME);

  //SetTunings(Kp, Ki, Kd, POn(Optional)) P_ON_E (Prop on Error) or P_ON_M (Prop on Measured)
  //GetKp(); GetKi(); GetKd(); GetMode(); GetDirection();

  Serial.begin(115200);

}

unsigned long interval = 0;
bool LEDStatus = false;
int LEDTime = 0;

void loop(){

  //Update encoder measured values
  UpdateFreqEncoderLeft();
  UpdateFreqEncoderRight();

  //Mix RC inputs to individual motor command levels
  MixCmds();

  //Convert motor commands to desired frequency
  CmdCalcRC();

  //Compute PIDs 
  LeftPID.Compute();
  RightPID.Compute();
  

  //Calculate outputs
  //Left Control
  LeftCmdPassive = map(AbsFreqSetpointLeft, CAL_X_MIN, CAL_X_MAX, CAL_Y_MIN, CAL_Y_MAX);
  LeftCmdTotal = OutputLeftPID + LeftCmdPassive;
  LeftCmdTotal = min(PWM_DUTY_MAX, LeftCmdTotal);
  
  if(LeftState == STOP){
    LeftCmdTotal = 0;
  }
  pwmWrite(PWM_LEFT_PIN, LeftCmdTotal);

  if(LeftState == FWD){
    digitalWrite(DIR_LEFT_PIN, LOW);
  } else {
    digitalWrite(DIR_LEFT_PIN, HIGH);
  }
  
  //Right Control
  RightCmdPassive = map(AbsFreqSetpointRight, CAL_X_MIN, CAL_X_MAX, CAL_Y_MIN, CAL_Y_MAX);
  RightCmdTotal = OutputRightPID + RightCmdPassive;
  RightCmdTotal = min(PWM_DUTY_MAX, RightCmdTotal);

  if(RightState == STOP){
    RightCmdTotal = 0;
  }
  pwmWrite(PWM_RIGHT_PIN, RightCmdTotal);

  if(RightState == FWD){
    digitalWrite(DIR_RIGHT_PIN, LOW);
  } else {
    digitalWrite(DIR_RIGHT_PIN, HIGH);
  }
  
  //Send parameters to plot
  if (millis() - interval >= 200) { 
    Serial.print(",");
    Serial.print(FwdRevCmd);
    Serial.print(",");
    Serial.print(LeftRightCmd);
    Serial.print(",");
    Serial.print(LeftCmdBilinear);
    Serial.print(",");
    Serial.print(RightCmdBilinear);
    Serial.print(",");
    Serial.print(FreqMeasuredLeft);
    Serial.print(",");
    Serial.println(FreqMeasuredRight);
    interval = millis();
   }

  //Blink LED to know that sketch is running
   if(millis() - LEDTime >= 1000){
     LEDTime = millis();
     if (LEDStatus) {
       digitalWrite(PC13,HIGH);
     } else {
       digitalWrite(PC13,LOW);
     }
     LEDStatus = !LEDStatus;
   }
}
