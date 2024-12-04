#ifndef _PROS_MAIN_H_

#define _PROS_MAIN_H_
#define PROS_USE_SIMPLE_NAMES
#define PROS_USE_LITERALS

#include "api.h"
#include "pros/apix.h"
#include "liblvgl/lvgl.h"

using namespace std;
using namespace pros;
using namespace adi;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Competition Methods */
void initialize(void);
void autonomous(void);
void opcontrol(void);

// Unused Competition Methods
void disabled(void);
void competition_initialize(void);

/* Autonomous Period Task */
void controlInputsAutonomous(void);

// Autonomous Programs
void runAutonomous(void);
void runProgram1(void);
void runProgram2(void);
void runProgram3(void);
void runProgram4(void);

// Autonomous Helper Methods
void moveForTime(vector<void (*)(int n)> targets, vector<int> values, int t);
void driveForTime(int r, int l, int t);
void driveForDistance(int r, int l, int d);
void intakeForTime(int i, int t);
void scoreRing(void);
void seizeGoal(void);
void releaseGoal(void);
void delayA(int n = 0);

/* Driver Controlled Period Task */
void controlInputsDriver(void);

// Driver Input Methods
void updateInputsDriver(void);
void updateToggles(void);
void updateTogglePID(void);
void updateTargets(void);
void updateTargetDrivetrain(void);
void updateTargetArm(void);
void updateTargetIntakeScore(void);
void updateTargetClamp(void);

/* Universal Period Task */
void controlInputsRobot(void);

// Robot Input Methods
void updateInputsRobot(void);
void updateActuals(void);
void updateActualDrivetrain(void);

/* Robot System Task */
void controlRobot(void);

// Robot Update Methods
void updateRobot(void);
void updateRobotDrivetrain(void);
void updateRobotArm(void);
void updateRobotIntakeScore(void);
void updateRobotClamp(void);

// Robot Helper Methods
int PID(int target, int sensor, double kP, double kI, double kD, int* I, int limitI, int* lastError);

/* Display Control System Task */
void controlDisplay(void);
void updateScreens(void);
void updateScreenTo(lv_obj_t * screen);
void updateGameInformation(void);
void updateImages(void);
void updateImageTo(lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom);
void updateCanvas(void);
void updateTexts(void);
void updateTextLabelTo(lv_obj_t **label, string text);
void updateTextTableTo(lv_obj_t **table, vector<int> widths, vector<vector<string>> text);
void updateController(void);

// Display Initializaiton Methods
void initializeAllGraphics(void);
void initializeColors(void);
void initializeFonts(void);
void initializeDSCs(void);
void initializeStyles(void);
void initializeAllScreens(void);
void initializeHeader(void);
void initializeScreenMain(void);
void initializeScreenAutonomous(void);
void initializeScreenElectronics(void);
void initializeScreenInformation(void);
void initializeText(void);

// Display Creation Methods
void createColor(lv_color_t *color, int hex);
void createFont(lv_font_t *font, lv_font_t source);
void createDscImg(lv_img_dsc_t *dsc, lv_img_dsc_t source);
void createDscRect(lv_draw_rect_dsc_t *dsc, lv_color_t bgColor, int borderWidth, lv_color_t borderColor);
void createScreen(lv_obj_t **screen);
void createTextStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor);
void createButtonStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor);
void createBoxStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor);
void createTableStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor);
void createCanvas(int screen, lv_obj_t **canvas, int x, int y, int w, int h);
void createImage(int screen, lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom);
void createLabel(int screen, lv_obj_t **label, int x, int y, lv_style_t *style);
void createDropdown(int screen, lv_obj_t **dropdown, string options, int x, int y, int w, int h, bool header);
void createButton(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y, void(*function)(lv_event_t *e));
void createBox(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y);
void createTable(int screen, lv_obj_t **table, int x, int y, int r, int c);
void createTableFormat(lv_event_t *e);

// Display Event Methods
void eventAutonomousManual(lv_event_t *e);

// Display Parsing Methods
string parseCurrentMode(void);
string parseCurrentPeriod(void);
string parseCurrentSide(void);
string parseCurrentPosition(void);
string parseCurrentAutonomous(void);
string parseCurrentScreen(void);

// Display Helper Methods
lv_obj_t* getScreenAt(int n);

/* Set Methods */
void setTargetDriveLeft(int n);
void setTargetDriveRight(int n);
void setTargetArm(int n);
void setTargetIntake(int n);
void setTargetScore(int n);
void setTargetClamp(int n);
void setTogglePID(bool b);
void setActualDriveLeft(int n);
void setActualDriveRight(int n);
void setCurrentState(bool b);
void setCurrentMode(int n);
void setCurrentPeriod(int n);
void setCurrentSide(int n);
void setCurrentPosition(int n);
void setCurrentAutonomous(int n);
void setCurrentScreen(int n);

/* Get Methods */
int getTargetDriveLeft(void);
int getTargetDriveRight(void);
int getTargetArm(void);
int getTargetIntake(void);
int getTargetScore(void);
int getTargetClamp(void);
int getActualDriveLeft(void);
int getActualDriveRight(void);
bool getTogglePID(void);
bool getCurrentState(void);
int getCurrentMode(void);
int getCurrentPeriod(void);
int getCurrentSide(void);
int getCurrentPosition(void);
int getCurrentAutonomous(void);
int getCurrentScreen(void);

/* Global Objects */
// Constants
const int waitInterval{12};
const int maxInterval{100};
const int doubleInputDelay{150};
const int autonDelay{300};

// Tasks
Mutex mutex;
Task periodAutonomous(controlInputsAutonomous);
Task periodDriverControlled(controlInputsDriver);
Task periodUniversal(controlInputsRobot);
Task systemRobot(controlRobot);
Task systemDisplay(controlDisplay);

// Controller
Controller masterController(E_CONTROLLER_MASTER);

// Electronics
MotorGroup motorsDriveLeft({-9, -10});
MotorGroup motorsDriveRight({1, 2});
MotorGroup motorsArm({3, -4});
MotorGroup motorIntake({7});
MotorGroup motorScore({8}); 
Pneumatics pneumaticClamp(8, true);

// Inputs
struct {
	int axis1{0};
	int axis2{0};
	int axis3{0};
	int axis4{0};
	int buttonX{0};
	int buttonA{0};
	int buttonB{0};
	int buttonY{0};
	int buttonUp{0};
	int buttonRight{0};
	int buttonDown{0};
	int buttonLeft{0};
	int buttonR1{0};
	int buttonR2{0};
	int buttonL1{0};
	int buttonL2{0};
} inputsDriver;
struct {
    int velocityL{0};
    int velocityR{0};
} inputsRobot;

// Target Variables
int targetDriveLeft{0};
int targetDriveRight{0};
int targetArm{0};
int targetIntake{0};
int targetScore{0};
int targetClamp{0};

// Actual Variables
int actualDriveLeft{0};
int actualDriveRight{0};

// Toggles
bool togglePID{false};

/* Graphical Objects */
// Constants
const int displayWidth{480};
const int displayHeight{240};

// Information
bool currentState{false};
int currentMode{0};
int currentPeriod{0};
int currentSide{0};
int currentPosition{0};
int currentAutonomous{0};
int currentScreen{0};

// Colors
lv_color_t colorWhite;
lv_color_t colorLightGray;
lv_color_t colorBlack;
lv_color_t colorDamienGreen;
lv_color_t colorSpartanGold;
lv_color_t colorMilenniumGold;
lv_color_t colorCoolGray;

// Fonts
lv_font_t fontTrajan40;
lv_font_t fontFauna14;
lv_font_t fontCalibri13;
lv_font_t fontCalibri11;

// DSCs
lv_img_dsc_t dscImgSpartanHelmetGreen;
lv_img_dsc_t dscImgSpartanHelmetGold;
lv_img_dsc_t dscImgLayoutVex;
lv_img_dsc_t dscImgLayoutSkills;
lv_draw_rect_dsc_t dscRectHeader;
lv_draw_rect_dsc_t dscRectBody;

// Styles
lv_style_t styleTextHeader1;
lv_style_t styleTextHeader2;
lv_style_t styleTextHeader3;
lv_style_t styleButton1;
lv_style_t styleButton1Selected;
lv_style_t styleButton2;
lv_style_t styleButton2Selected;
lv_style_t styleBox;
lv_style_t styleTableCell;

// Screens
vector<vector<lv_obj_t *>> screenObjects;
lv_obj_t *screenMainMenu;
lv_obj_t *screenAutonomous;
lv_obj_t *screenElectronics;
lv_obj_t *screenInformation;

// Header
lv_obj_t *imgHeader;
lv_obj_t *canvasHeader;
lv_obj_t *textHeader1;
lv_obj_t *textHeader2;
lv_obj_t *dropdownHeader;

// Main Menu Screen
lv_obj_t *buttonMainInfo;
lv_obj_t *textMainInfo;
lv_obj_t *imgMainLayout;

// Autonomous Screen
lv_obj_t *dropdownAutonomousMode;
lv_obj_t *dropdownAutonomousSide;
lv_obj_t *dropdownAutonomousPosition;
lv_obj_t *dropdownAutonomousAutonomous;
lv_obj_t *buttonAutonomousManual;
lv_obj_t *textAutonomousManual;

// Electronics Screen
lv_obj_t *tableElectronicsMotors;
lv_obj_t *tableElectronicsADI;

// Information Screen
lv_obj_t *buttonInfoGame;
lv_obj_t *textInfoGame;
lv_obj_t *buttonInfoCoaches;
lv_obj_t *textInfoCoaches;
lv_obj_t *buttonInfoCode;
lv_obj_t *textInfoCode;
lv_obj_t *tableInfoTeam;

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#endif // __cplusplus

#endif // _PROS_MAIN_H_