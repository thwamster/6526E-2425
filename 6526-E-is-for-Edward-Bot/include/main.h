#ifndef _PROS_MAIN_H_

#define _PROS_MAIN_H_
#define PROS_USE_SIMPLE_NAMES
#define PROS_USE_LITERALS

#include "api.h"
#include "pros/apix.h"
#include "liblvgl/lvgl.h"

using namespace std;
using namespace pros;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Competition */
void initialize();
void autonomous();
void opcontrol();

// Unused Competition Methods
void disabled();
void competition_initialize();

/* Autonomous Period Task */
void getAutonomousInputs();

// Autonomous Programs
void runAutonomous();
void runProgram1();
void runProgram2();
void runProgram3();
void runProgram4();

// Autonomous Helper Methods
void moveForTime(std::vector<int*> targets, std::vector<int> values, int t);
void driveForTime(int r, int l, int t);
void intakeForTime(int i, int t);
void scoreRing();
void seizeGoal();
void releaseGoal();
void delayA(int n = 0);

/* Driver Controlled Period Task */
void getDriverInputs();

/* Robot Control System Task */
void updateRobot();

// Robot Helper Methods
int PID(int target, int sensor, double kP, double kI, double kD, int* I, int limitI, int* lastError);

/* Screen Control System Task */
void updateDisplay(void);
void checkUpdates();
void updateScreens(lv_obj_t * screen);
void updateText(void);
void updateCanvas(void);
void updateController(void);

// Screen Parsing Methods
std::string parseCurrentPeriod(void);
std::string parseCurrentSide(void);
std::string parseCurrentPosition(void);
std::string parseCurrentAutonomous(void);

/* Graphical Methods */
void initializeAllGraphics(void);
void initializeScreens(void);
void initializeColors(void);
void initializeFonts(void);
void initializeImageDSCs(void);
void initializeStyles(void);
void initializeDSC(void);
void initializeCanvas(void);
void initializeDropdown(void);
void initializeText(void);
void initializeButtons(void);
void initializeImages(void);

/* Set Methods */
void setCurrentPeriod(int n);
void setCurrentSide(int n);
void setCurrentPosition(int n);
void setCurrentAutonomous(int n);
void setLeftDriveTarget(int n);
void setRightDriveTarget(int n);
void setIntakeScoreTarget(int n);
void setPistonTarget(int n);
void setEnablePID(bool b);
void setLeftDriveActual(int n);
void setRightDriveActual(int n);

/* Get Methods */
int getCurrentPeriod(void);
int getCurrentSide(void);
int getCurrentPosition(void);
int getCurrentAutonomous(void);
int getLeftDriveTarget(void);
int getRightDriveTarget(void);
int getIntakeScoreTarget(void);
int getPistonTarget(void);
int getLeftDriveActual(void);
int getRightDriveActual(void);
bool getEnablePID(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#endif // __cplusplus

#endif // _PROS_MAIN_H_