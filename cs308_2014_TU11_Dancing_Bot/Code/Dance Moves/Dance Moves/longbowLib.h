#ifndef __LONGBOWLIB_H  
#define  __LONGBOWLIB_H
	
// servo number and angle call function
angle_1A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_1B (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_1C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_2A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_2B (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_2C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_3A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_3B (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_3C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_4A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_4B (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_4C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_5A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_5B (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_5C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_6A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_6A (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_6C (unsigned char );             // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_19 (unsigned char );  // pan servo  // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)
angle_20 (unsigned char );  // tilt servo   // specify the angle in degree as an argument (limit: 5 to 175 degree for safety operation)

void robot_arm_calibration  (void);										// call for all servo motors to set for 90 position
void robot_stand_position (void);                                       // call this for robot to stand at less current consumption position

void setServoPosition(unsigned char ,unsigned char );// call this to move individual servo motor by passing servo number and final angle to move

void init_devices(void);                                                // initializing  Microcontroller 20 servo pins used, timer 1 

#endif