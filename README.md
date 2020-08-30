# DualPIDMotorControl
Dual PID control using a STM32Duino with encoders and a Cytron SmartDriveDuo motor controller

Features
- Read PPM Servo signals from an RC Receiver.
- Mixes single stick of RC Transmitter to individual motor outputs.

Toolchain
- Arduino IDE
- Roger Clark Melborne version of STM32Duino - https://github.com/rogerclarkmelbourne/Arduino_STM32
- Rotary Encoder Library https://github.com/enjoyneering/RotaryEncoder
- PIDController Library

BOM:
1. "Bluepill" STM32 controller - EBay or Bangood
	- Programmer of some sort is also needed.  Serial adaptor, STLink V2, etc.
2. Cytron SmartDriveDuo-30 - https://www.cytron.io/p-30amp-7v-35v-smartdrive-dc-motor-driver-2-channels
3. 600 pulse/rev encoder - EBay
4. RC Receiver
5. RC Transmitter

