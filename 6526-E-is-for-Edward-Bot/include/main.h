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
void getAutonomousInputs(void);

// Autonomous Programs
void runAutonomous(void);
void runProgram1(void);
void runProgram2(void);
void runProgram3(void);
void runProgram4(void);

// Autonomous Helper Methods
void moveForTime(vector<int*> targets, vector<int> values, int t);
void driveForTime(int r, int l, int t);
void intakeForTime(int i, int t);
void scoreRing(void);
void seizeGoal(void);
void releaseGoal(void);
void delayA(int n = 0);

/* Driver Controlled Period Task */
void getDriverInputs(void);

/* Robot Control System Task */
void updateRobot(void);

// Robot Helper Methods
int PID(int target, int sensor, double kP, double kI, double kD, int* I, int limitI, int* lastError);

/* Display Control System Task */
void updateDisplay(void);
void updateScreen(void);
void updateScreenTo(lv_obj_t * screen);
void updateGameInformation(void);
void updateImages(void);
void updateImageTo(lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom);
void updateCanvas(void);
void updateText(void);
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

// Display Creation Methods
void createColor(lv_color_t *color, int hex);
void createFont(lv_font_t *font, lv_font_t source);
void createDscImg(lv_img_dsc_t *dsc, lv_img_dsc_t source);
void createDscRect(lv_draw_rect_dsc_t *dsc, lv_color_t bgColor, int borderWidth, lv_color_t borderColor);
void createScreen(lv_obj_t **screen);
void createTextStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor);
void createButtonStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor);
void createBoxStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor);
void createCanvas(int screen, lv_obj_t **canvas, int x, int y, int w, int h);
void createImage(int screen, lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom);
void createLabel(int screen, lv_obj_t **label, int x, int y, lv_style_t *style);
void createDropdown(int screen, lv_obj_t **dropdown, string options, int x, int y, int w, int h);
void createBox(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y);

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
void setLeftDriveTarget(int n);
void setRightDriveTarget(int n);
void setIntakeScoreTarget(int n);
void setPistonTarget(int n);
void setEnablePID(bool b);
void setLeftDriveActual(int n);
void setRightDriveActual(int n);
void setCurrentState(bool b);
void setCurrentMode(int n);
void setCurrentPeriod(int n);
void setCurrentSide(int n);
void setCurrentPosition(int n);
void setCurrentAutonomous(int n);
void setCurrentScreen(int n);

/* Get Methods */
int getLeftDriveTarget(void);
int getRightDriveTarget(void);
int getIntakeScoreTarget(void);
int getPistonTarget(void);
int getLeftDriveActual(void);
int getRightDriveActual(void);
bool getEnablePID(void);
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
Task autonomousPeriod(getAutonomousInputs);
Task driverControlledPeriod(getDriverInputs);
Task robotControlSystem(updateRobot);
Task displayControlSystem(updateDisplay);

// Controller
Controller masterController(E_CONTROLLER_MASTER);

// Electronics
MotorGroup leftDriveMotors({-9, -10});
MotorGroup rightDriveMotors({1,2});
MotorGroup intakeMotor({7});
MotorGroup scoreMotor({8}); 
Pneumatics clampPistons(8, true);
// TODO: Sensors

// Target Variables
int leftDriveTarget{0};
int rightDriveTarget{0};
int intakeScoreTarget{0};
int pistonTarget{0};

// Actual Variables
int leftDriveActual{0};
int rightDriveActual{0};

// Toggles
bool enablePID{false};

/* Graphical Objects */
// Constants
const int displayWidth{480};
const int displayHeight{240};
static lv_color_t bufferAll[LV_CANVAS_BUF_SIZE_TRUE_COLOR(480, 240)];

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
lv_style_t styleButton;
lv_style_t styleButtonSelected;
lv_style_t styleBox;

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

// Electronics Screen
// TODO: Implement Electronics Screen

// Information Screen
// TODO: Implement Information Screen

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#endif // __cplusplus

#endif // _PROS_MAIN_H_