#ifndef _PROS_MAIN_H_

#define _PROS_MAIN_H_
#define PROS_USE_SIMPLE_NAMES
#define PROS_USE_LITERALS
#include "api.h"
#ifdef __cplusplus

extern "C" {
#endif // __cplusplus

/* Competition Methods */
void initialize();
void disabled();
void competition_initialize();
void autonomous();
void opcontrol();

/* Autonomous Period Task */
void runAutonomousInputs();
int selectAutonomous();
void moveForTime(std::vector<pros::MotorGroup*> motors, std::vector <int> values, int t);

/* Driver Controlled Period Task */
void getDriverInputs();

/* Robot Control System Task */
void updateRobot();
int PID(int target, int sensor, double kP, double kI, double kD, int* I, int limitI, int* lastError);

/* Debug Task */
void updateDisplay();
void displayClear();
void resetAllVariables();
void on_center_button();

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#endif // __cplusplus

#endif // _PROS_MAIN_H_