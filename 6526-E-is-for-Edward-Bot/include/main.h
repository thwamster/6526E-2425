#ifndef _PROS_MAIN_H_

#define _PROS_MAIN_H_
#define PROS_USE_SIMPLE_NAMES
#define PROS_USE_LITERALS

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Competition */
void initialize();
void autonomous();
void opcontrol();

// Competition Helper Methods
void updateGameInformation();

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
void updateDisplay();
void updateDisplayText();
void updateScreenColors(pros::Color pen, pros::Color eraser);

// Screen Drawing Methods
void drawScreenBackground();
void drawScreenButton(std::string text, int x1, int y1, int x2, int y2, pros::Color pen, pros::Color eraser);
void drawScreenText(std::string text, int x, int y, pros::Color pen, pros::Color eraser);
void drawScreenHeader();
void drawScreenData();
void drawController();
int drawScreenButtons(std::string prompt, std::vector<std::string> icons, std::vector<pros::Color> colors);
int waitUntilButtonPressed(int n, int width);

// Screen Parsing Methods
std::string parseCurrentPeriod();
std::string parseCurrentSide();
std::string parseCurrentPosition();
std::string parseCurrentAutonomous();

// Screen Helpr Methods
int reverseScreenWidth(int n);
int reverseScreenHeight(int n);
bool isBetween(int n, int min, int max);

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
void setDisplayTextAt(int i, std::string s);

/* Get Methods */
int getCurrentPeriod();
int getCurrentSide();
int getCurrentPosition();
int getCurrentAutonomous();
int getLeftDriveTarget();
int getRightDriveTarget();
int getIntakeScoreTarget();
int getPistonTarget();
int getLeftDriveActual();
int getRightDriveActual();
bool getEnablePID();
std::string getDisplayTextAt(int i);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#endif // __cplusplus

#endif // _PROS_MAIN_H_