#include "main.h"

/* Global Objects */
// Constant Intervals
const int waitInterval = 12;
const int maxInterval = 100;
const int doubleInputDelay = 150;
const int autonDelay = 300;

// Tasks
pros::Mutex mutex;
pros::Task autonomousPeriod(getAutonomousInputs);
pros::Task driverControlledPeriod(getDriverInputs);
pros::Task robotControlSystem(updateRobot);
pros::Task screenControlSystem(updateDisplay);

// Controller
pros::Controller masterController(E_CONTROLLER_MASTER);

// Robot Movement
pros::MotorGroup leftDriveMotors({-9, -10});
pros::MotorGroup rightDriveMotors({1,2});
pros::MotorGroup intakeMotor({7});
pros::MotorGroup scoreMotor({8}); 
adi::Pneumatics clampPistons(8, true);

// Target Variables
int leftDriveTarget{};
int rightDriveTarget{};
int intakeScoreTarget{};
int pistonTarget{};

// Actual Variables
int leftDriveActual{};
int rightDriveActual{};

// Toggles
bool enablePID{false};

/* Graphical Objects */

// Game Information
int currentPeriod{};
int currentSide{};
int currentPosition{};
int currentAutonomous{};

int currentScreen{};
bool initialized{false};

// Buffers
static lv_color_t bufferAll[LV_CANVAS_BUF_SIZE_TRUE_COLOR(480, 240)];

// Colors
lv_color_t colorWhite;
lv_color_t colorBlack;
lv_color_t colorDamienGreen;
lv_color_t colorSpartanGold;
lv_color_t colorMilenniumGold;
lv_color_t colorCoolGray;

// Screens
lv_obj_t *screenMainMenu;
lv_obj_t *screenAutonomous;
lv_obj_t *screenElectronics;
lv_obj_t *screenInformation;

// Fonts
lv_font_t fontTrajan40;
lv_font_t fontFauna14;
lv_font_t fontCalibri13;
lv_font_t fontCalibri11;

// Images
lv_obj_t *imgHeader;
lv_obj_t *imgMainLayout;

// Image DSCs
lv_img_dsc_t imgDscSpartanHelmetGreen;
lv_img_dsc_t imgDscSpartanHelmetGold;
lv_img_dsc_t imgDscLayoutVex;
lv_img_dsc_t imgDscLayoutSkills;

// Styles
static lv_style_t styleTextHeader1;
static lv_style_t styleTextHeader2;
static lv_style_t styleTextHeader3;
static lv_style_t styleButton;
static lv_style_t styleButtonSelected;
static lv_style_t styleBox;

// Rectangle DSCs
lv_draw_rect_dsc_t dscHeader;
lv_draw_rect_dsc_t dscBody;

// Canvas
lv_obj_t *canvasHeader;

// Dropdown
lv_obj_t * dropdownHeader;

// Texts
lv_obj_t *textHeader1;
lv_obj_t *textHeader2;
lv_obj_t *textBattery1;
lv_obj_t *textBattery2;
lv_obj_t *textMainInfo;

// Buttons
lv_obj_t *buttonMainInfo;

/* Competition */
void initialize() {

	// Updsate graphics
	initializeAllGraphics();
	initialized = true;

	// Update tasks
	autonomousPeriod.suspend();
	driverControlledPeriod.suspend();
	robotControlSystem.suspend();
	screenControlSystem.resume();

	setCurrentPeriod(0); // Update period
}

void autonomous() {
	
	// Update tasks
	autonomousPeriod.resume();
	driverControlledPeriod.suspend();
	robotControlSystem.resume();

	setCurrentPeriod(1);

	// Control loop
	while (true) {
		
		delay(waitInterval); // Wait
	}
}

void opcontrol() {

	// Update Tasks
	autonomousPeriod.suspend();
	driverControlledPeriod.resume();
	robotControlSystem.resume();

	setCurrentPeriod(2);

	// Control loop
	while (true) {
		
		delay(waitInterval); // Wait
	}
}

// Unused Competition Methods
void disabled() {}	

void competition_initialize() {}

/* Autonomous Period Task */
void getAutonomousInputs() {
	bool b{true};

	// Control loop
	while (true) {
		
		// Autonomous loop
		if (b) {
			
			// Select correct autonomous program
			runAutonomous();
			b = false;
		}

		delay(waitInterval); // Waits
	}
}

// Autonomous Programs
void runAutonomous() {
	switch (1) {
		case 0:  // Do Nothing
			delayA();
			break;

		case 1: // Program #1: Preload Score
			runProgram1();
			break;
		
		case 2: // Program #2:
			runProgram2();
			break;
		
		case 3: // Program #3:
			runProgram3();
			break;

		case 4: // Program #4:
			runProgram4();
			break;
		
	}
}

void runProgram1() {
	delayA(1000);
	driveForTime(-127, -127, 320);
	delayA(1000);
	seizeGoal();
	delayA(1000);
	scoreRing();
	delayA(1000);
	driveForTime(-80, -80, 1000);
	delayA(1000);
	driveForTime(-20, -20, 2000);
	delayA(3000);
}

void runProgram2() {}

void runProgram3() {}

void runProgram4() {}

// Autonomous Helper Methods
void moveForTime(std::vector<int*> targets, std::vector<int> values, int t) {

	// Updates all values
	for (int i = 0; i < targets.size() && i < values.size(); i++) {
		*targets[i] = values[i];
	}

	delay(t); // Wait

	// Resets all values
	for (int i = 0; i < targets.size() && i < values.size(); i++) {
		*targets[i] = 0;
	}
}

void driveForTime(int r, int l, int t) { moveForTime({&rightDriveTarget, &leftDriveTarget}, {r, l}, t); }
void intakeForTime(int i, int t) { moveForTime({&intakeScoreTarget}, {i}, t); }
void scoreRing() { intakeForTime(1, 2500); }
void seizeGoal() { setPistonTarget(1); }
void releaseGoal() { setPistonTarget(0); }
void delayA(int n) { (n < autonDelay) ? delay(autonDelay) : delay(n); }

/* Driver Controlled Period Task */
void getDriverInputs() {

	// Raw controller values
	int axis1{};
	int axis2{};
	int axis3{};
	int axis4{};
	int buttonX;
	int buttonA{};
	int buttonB{};
	int buttonY{};
	int buttonUp{};
	int buttonRight{};
	int buttonDown{};
	int buttonLeft{};
	int buttonR1{};
	int buttonR2{};
	int buttonL1{};
	int buttonL2{};

	// Raw sensor values
	int velocityR{};
	int velocityL{};

	// Delays
	int aDelayCount{};

	// Control Loop
	while (true) {

		// Receives controller inputs
		axis1 = masterController.get_analog(ANALOG_RIGHT_X);
		axis2 = masterController.get_analog(ANALOG_RIGHT_Y);
		axis3 = masterController.get_analog(ANALOG_LEFT_Y);
		axis4 = masterController.get_analog(ANALOG_LEFT_X);
		buttonX = masterController.get_digital(DIGITAL_X);
		buttonA = masterController.get_digital(DIGITAL_A);
		buttonB = masterController.get_digital(DIGITAL_B);
		buttonY = masterController.get_digital(DIGITAL_Y);
		buttonUp = masterController.get_digital(DIGITAL_UP);
		buttonRight = masterController.get_digital(DIGITAL_RIGHT);
		buttonDown = masterController.get_digital(DIGITAL_DOWN);
		buttonLeft = masterController.get_digital(DIGITAL_LEFT);
		buttonR1 = masterController.get_digital(DIGITAL_R1);
		buttonR2 = masterController.get_digital(DIGITAL_R2);
		buttonL1 = masterController.get_digital(DIGITAL_L1);
		buttonL2 = masterController.get_digital(DIGITAL_L2);

		// Recieves sensor inputs
		velocityR = rightDriveMotors.get_actual_velocity(0);
		velocityL = leftDriveMotors.get_actual_velocity(0);

		mutex.take(maxInterval); // Begin Exclusivity

		// Sets Targets
		setLeftDriveTarget(axis3);
		setRightDriveTarget(axis2);

		if (buttonA == 1 && aDelayCount >= doubleInputDelay) {
			setEnablePID(!getEnablePID);
			aDelayCount = 0;
		}
		aDelayCount++;
		
		if (buttonUp == 1 && buttonDown == 0) {
			setPistonTarget(1);
		}
		else if (buttonUp == 0 && buttonDown == 1){
			setPistonTarget(0);
		}

		if (buttonR1 == 1 && buttonR2 == 0) {
			setIntakeScoreTarget(1);
		}
		else if (buttonR1 == 0 && buttonR2 == 1) {
			setIntakeScoreTarget(-1);
		}
		else {
			setIntakeScoreTarget(0);
		}

		// Sets actuals
		setRightDriveActual(velocityR);
		setLeftDriveActual(velocityL);

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

/* Robot Control System Task */
void updateRobot() {
	int rightDriveI = 0;
	int rightDriveLastError = 0;
	int leftDriveI = 0;
	int leftDriveLastError = 0;

	// Control loop
	while (true) {

		mutex.take(maxInterval); // Begin Exclusivity

		if (getEnablePID()) {
			rightDriveMotors.move(PID(getRightDriveTarget(), getRightDriveActual(), 0.6, 0.2, 0.2, &rightDriveI, 10, &rightDriveLastError));
			leftDriveMotors.move(PID(getLeftDriveTarget(), getLeftDriveActual(), 0.6, 0.2, 0.2, &leftDriveI, 10, &leftDriveLastError));
		}
		else {
			rightDriveMotors.move(getRightDriveTarget());
			leftDriveMotors.move(getLeftDriveTarget());
		}

		if (getPistonTarget() == 1) {
			clampPistons.extend();
		}
		else {
			clampPistons.retract();
		}

		if (getIntakeScoreTarget() == 1) {
			intakeMotor.move(90);
			scoreMotor.move(80);
		}
		else if (getIntakeScoreTarget() == -1) {
			intakeMotor.move(-90);
			scoreMotor.move(-80);
			
		}
		else if (getIntakeScoreTarget() == 0) {
			intakeMotor.move(0);
			scoreMotor.move(0);
		}

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

// Robot Helper Methods
int PID(int target, int sensor, double kP, double kI, double kD, int* I, int limitI, int* lastError) {
	int PID = 0;
	int error = target - sensor;

	// Calculates integral
	if (abs(error) < limitI) {
		*I += error;
	}
	else {
		*I = 0;
	}

	// Calculates PID
	PID = (int) ((kP * error) + (kI + (*I)) + (kD * (error - (*lastError))));

	// Updates last error
	*lastError = error;

	return PID; // Returns
}

/* Screen Control System Task */
void updateDisplay() {
	int bgDelay{};

	// Control loop
	while (true) {

		// If during driver controlled or autonomous periods
		if (getCurrentPeriod() == 2 || getCurrentPeriod() == 3) {
			updateController();
		}

		if (initialized) { 
			checkUpdates();
			updateText();
			updateCanvas();
		}

		delay(waitInterval); // Wait
	}
}

void checkUpdates() {
	int n = lv_dropdown_get_selected(dropdownHeader);
	if (n == currentScreen) {
		return;
	}
	else {
		currentScreen = n;
		switch (n) {
			case 0:
				updateScreens(screenMainMenu);
				break;
			case 1:
				updateScreens(screenAutonomous);
				break;
			case 2:
				updateScreens(screenElectronics);
				break;
			case 3:
				updateScreens(screenInformation);
				break;
		}
	}
}

void updateScreens(lv_obj_t * screen) {
	lv_scr_load(screen);
	lv_obj_set_parent(canvasHeader, screen);
	lv_obj_set_parent(dropdownHeader, screen);
	lv_obj_set_parent(textHeader1, screen);
	lv_obj_set_parent(textHeader2, screen);
	lv_obj_set_parent(imgHeader, screen);
	lv_obj_set_parent(buttonMainInfo, screenMainMenu);
	lv_obj_set_parent(imgMainLayout, screenMainMenu);
}

void updateText() {
	lv_label_set_text(textHeader1, "6526-E");
	lv_label_set_text(textHeader2, "E-is-for-Edward\nDamien Robotics");
	string stringInfo = "Period: " + parseCurrentPeriod() + "\nStart Position: " + parseCurrentPosition() + "\nSelected Autonomous: " + parseCurrentAutonomous();
    lv_label_set_text(textMainInfo, stringInfo.c_str());
}

void updateController() {
	masterController.print(0, 0, ("6526-E: " + parseCurrentSide()).c_str());
}

// Screen Parsing Methods
std::string parseCurrentPeriod() {
	switch (getCurrentPeriod()) {
		case 1: return "Initializing";
		case 2: return "Driver Controlled";
		case 3: return "Autonomous Controlled";
		default: return "Unknown";
	}
}

std::string parseCurrentSide() {
	switch (getCurrentSide()) {
		case 1: return "Red";
		case 2: return "Blue";
		default: return "Unknown";
	}
}

std::string parseCurrentPosition() {
	switch (getCurrentPosition()) {
		case 1: return "Positive";
		case 2: return "Negative";
		default: return "Unknown";
	}
}

std::string parseCurrentAutonomous() {
	switch (getCurrentAutonomous()) {
		case 1: return "Program #1: Preload Score";
		case 2: return "Program #2";
		case 3: return "Program #3";
		case 4: return "Program #4";
		default: return "Unknown";
	}
}

/* Graphical Methods */
void initializeAllGraphics() {
	initializeScreens();
	initializeColors();
	initializeFonts();
	initializeImageDSCs();
	initializeStyles();
	initializeDSC();
	initializeCanvas();
	initializeDropdown();
	initializeText();
	initializeButtons();
	initializeImages();
	
	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
}

void initializeScreens() {
	screenMainMenu = lv_obj_create(NULL);
	screenAutonomous = lv_obj_create(NULL);
	screenElectronics = lv_obj_create(NULL);
	screenInformation = lv_obj_create(NULL);

	lv_disp_load_scr(screenMainMenu);
}

void initializeColors() {
	colorWhite = lv_color_hex(0xFFFFFF);
	colorBlack = lv_color_hex(0x000000);
    colorDamienGreen = lv_color_hex(0x244546);
    colorSpartanGold = lv_color_hex(0xFFB81C);
    colorMilenniumGold = lv_color_hex(0xB3A369);
    colorCoolGray = lv_color_hex(0xA7A8A9);
}

void initializeFonts() {
	LV_FONT_DECLARE(trajan_40);
	LV_FONT_DECLARE(fauna_14);
	LV_FONT_DECLARE(calibri_13);
	LV_FONT_DECLARE(calibri_11);

	fontTrajan40 = trajan_40;
	fontFauna14 = fauna_14;
	fontCalibri13 = calibri_13;
	fontCalibri11 = calibri_11;
}

void initializeImageDSCs() {
	LV_IMG_DECLARE(icon_spartan_helmet);
	LV_IMG_DECLARE(icon_spartan_helmet_gold);
	LV_IMG_DECLARE(layout_vex);
	LV_IMG_DECLARE(layout_skills);

	imgDscSpartanHelmetGreen = icon_spartan_helmet;
	imgDscSpartanHelmetGold = icon_spartan_helmet_gold;
	imgDscLayoutVex = layout_vex;
	imgDscLayoutSkills = layout_skills;
}

void initializeStyles() {
	lv_style_init(&styleTextHeader1);
	lv_style_set_text_font(&styleTextHeader1, &fontTrajan40);
    lv_style_set_text_color(&styleTextHeader1, colorSpartanGold);

    lv_style_init(&styleTextHeader2);
	lv_style_set_text_font(&styleTextHeader2, &fontFauna14);
    lv_style_set_text_color(&styleTextHeader2, colorSpartanGold);

	lv_style_init(&styleTextHeader3);
	lv_style_set_text_font(&styleTextHeader3, &fontCalibri13);
    lv_style_set_text_color(&styleTextHeader3, colorBlack);

	lv_style_init(&styleButton); 
	lv_style_set_text_font(&styleButton, &fontFauna14);
	lv_style_set_text_color(&styleButton, colorSpartanGold);
	lv_style_set_bg_color(&styleButton, colorDamienGreen);
	lv_style_set_radius(&styleButton, 0);
	lv_style_set_border_width(&styleButton, 3);
	lv_style_set_border_color(&styleButton, colorBlack);

	lv_style_init(&styleBox);
	lv_style_set_text_font(&styleBox, &fontCalibri13);
	lv_style_set_text_color(&styleBox, colorBlack);
	lv_style_set_pad_all(&styleBox, 4);
	lv_style_set_bg_color(&styleBox, colorCoolGray);
	lv_style_set_radius(&styleBox, 0);
	lv_style_set_border_width(&styleBox, 2);
	lv_style_set_border_color(&styleBox, colorBlack);
}

void initializeDSC() {
	lv_draw_rect_dsc_init(&dscHeader);
    dscHeader.bg_color = colorDamienGreen;
    dscHeader.border_color = colorBlack;
    dscHeader.border_width = 3;

	lv_draw_rect_dsc_init(&dscBody);
    dscBody.bg_color = colorCoolGray;
    dscBody.border_color = colorBlack;
    dscBody.border_width = 3;
}

void initializeCanvas() {
	canvasHeader = lv_canvas_create(lv_scr_act());
	lv_obj_align(canvasHeader, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(canvasHeader, bufferAll, 480, 240, LV_IMG_CF_TRUE_COLOR);
}

void updateCanvas() {
	lv_canvas_fill_bg(canvasHeader, colorCoolGray, LV_OPA_COVER);
	lv_canvas_draw_rect(canvasHeader, 0, 0, 480, 50, &dscHeader);
	lv_canvas_draw_rect(canvasHeader, 0, 50, 480, 230, &dscBody);
}

void initializeDropdown() {
	dropdownHeader = lv_dropdown_create(lv_scr_act());
	lv_dropdown_set_options(dropdownHeader, "Main Menu\nAutonomous\nElectronics\nInformation");
	lv_dropdown_set_symbol(dropdownHeader, NULL);
	lv_obj_add_style(dropdownHeader, &styleButton, 0);
	lv_obj_add_style(lv_dropdown_get_list(dropdownHeader), &styleButton, 0);
	lv_obj_set_size(dropdownHeader, 130, 40);
	lv_obj_align(dropdownHeader, LV_ALIGN_TOP_LEFT, 345, 5);
}

void initializeText() {
	textHeader1 = lv_label_create(lv_scr_act());
    lv_obj_add_style(textHeader1, &styleTextHeader1, 0);
    lv_obj_align(textHeader1, LV_ALIGN_TOP_LEFT, 43, 0);

    textHeader2 = lv_label_create(lv_scr_act());
    lv_obj_add_style(textHeader2, &styleTextHeader2, 0);
    lv_obj_align(textHeader2, LV_ALIGN_TOP_LEFT, 185, 5);
}

void initializeButtons() {
	buttonMainInfo = lv_btn_create(screenMainMenu);
	lv_obj_align(buttonMainInfo, LV_ALIGN_TOP_LEFT, 10, 60);
	lv_obj_add_style(buttonMainInfo, &styleBox, 0);
	lv_obj_clear_flag(buttonMainInfo, LV_OBJ_FLAG_CLICKABLE);

	textMainInfo = lv_label_create(buttonMainInfo);
    lv_obj_add_style(textMainInfo, &styleTextHeader3, 0);
}

void initializeImages() {
	imgHeader = lv_img_create(lv_scr_act());
	lv_img_set_src(imgHeader, &imgDscSpartanHelmetGold);
	lv_img_set_pivot(imgHeader, 0, 0);
	lv_img_set_zoom(imgHeader, ((int) (256 * 0.22)));
	lv_obj_align(imgHeader, LV_ALIGN_TOP_LEFT, 5, 5);

	imgMainLayout = lv_img_create(lv_scr_act());
	lv_img_set_src(imgMainLayout, &imgDscLayoutVex);
	lv_img_set_pivot(imgMainLayout, 0, 0);
	lv_img_set_zoom(imgMainLayout, ((int) (256 * 0.64)));
	lv_obj_align(imgMainLayout, LV_ALIGN_TOP_LEFT, 295, 55);
}

/* Set Methods */
void setCurrentPeriod(int n) { currentPeriod = n; }
void setCurrentSide(int n) { currentSide = n; }
void setCurrentPosition(int n) { currentPosition = n; }
void setCurrentAutonomous(int n) { currentAutonomous = n; }
void setLeftDriveTarget(int n) { leftDriveTarget = n; }
void setRightDriveTarget(int n) { rightDriveTarget = n; }
void setIntakeScoreTarget(int n) { intakeScoreTarget = n; }
void setPistonTarget(int n) { pistonTarget = n; }
void setEnablePID(bool b) { enablePID = b; }
void setLeftDriveActual(int n) { leftDriveActual = n; }
void setRightDriveActual(int n) { rightDriveActual = n; }

/* Get Methods */
int getCurrentPeriod() { return currentPeriod; }
int getCurrentSide() { return currentSide; }
int getCurrentPosition() { return currentPosition; }
int getCurrentAutonomous() { return currentAutonomous; }
int getLeftDriveTarget() { return leftDriveTarget; }
int getRightDriveTarget() { return rightDriveTarget; }
int getIntakeScoreTarget() { return intakeScoreTarget; }
int getPistonTarget() { return pistonTarget; }
int getLeftDriveActual() { return leftDriveActual; }
int getRightDriveActual() { return rightDriveActual; }
bool getEnablePID() { return enablePID; }