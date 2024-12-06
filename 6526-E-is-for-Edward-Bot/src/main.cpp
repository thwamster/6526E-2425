#include "main.h"

/* Competition Methods */
void initialize() {

	// Update graphics
	initializeAllGraphics();
	setCurrentState(true);

	// Update tasks
	periodUniversal.resume();
	periodAutonomous.suspend();
	periodDriverControlled.suspend();
	systemRobot.suspend();
	systemDisplay.resume();

	setCurrentPeriod(0); // Update period
}

void autonomous() {
	
	// Update tasks
	periodAutonomous.resume();
	periodDriverControlled.suspend();
	systemRobot.resume();

	setCurrentPeriod(1); // Update period

	// Control loop
	while (true) {
		
		delay(waitInterval); // Wait
	}
}

void opcontrol() {

	// Update Tasks
	periodAutonomous.suspend();
	periodDriverControlled.resume();
	systemRobot.resume();

	setCurrentPeriod(2); // Update period

	// Control loop
	while (true) {
		
		delay(waitInterval); // Wait
	}
}

// Unused Competition Methods
void disabled() {}	

void competition_initialize() {}

/* Autonomous Period Task */
void controlInputsAutonomous() {
	bool b{true};

	// Control loop
	while (true) {
		
		// Autonomous loop, runs only once
		if (getCurrentAutonomous()) {
			
			// Select correct autonomous program
			runAutonomous();
			b = false;
		}

		delay(waitInterval); // Waits
	}
}

// Autonomous Programs
void runAutonomous() {

	setToggleBrake(true); // Updates brakes

	// Select autonomous program
	switch (getCurrentAutonomous()) {
		
		case 1: // Program #1: Drive Backward
			runProgram1();
			break;
		
		case 2: // Program #2: 2-Ring Score
			runProgram2();
			break;
		
		case 3: // Program #3: [n/a]
			runProgram3();
			break;

		case 4: // Program #4: Testing
			runProgram4();
			break;

		default: // Do Nothing
			delayA();
			break;
	}
}

void runProgram1() {
	//BLUE SIDE TEST ONLY
	
	//drive backwards onto the goal spot, clamp goal, and score ring
	delayA();
	driveForTime(-127,-127,600);
	delayA();
	seizeGoal();
	delayA();
	scoreRing();
	delayA();

	//turn to face the 2 stack to the right, run onto it, and score ring
	driveForTime(-127,127,405);
	delayA();
	driveAndIntakeForTime(127,127,90,127,500);
	delayA();
	intakeForTime(90, 127, 3000);	
	delayA();

	//turn left, drive into a line stack, and score the ring
	driveForTime(127,-127,405);
	delayA();
	driveAndIntakeForTime(80,80,90,127,200); //make this more precise
	delayA();
	intakeForTime(90, 127, 3000);
	delayA();
}

void runProgram2() {
	// Off starting line
	delayA();
	releaseGoal();
	delayA(500);
	driveForTime(-100, -100, parseDistanceToTime(100, 10)); // Drive backward 10 centimeters
	delayA();
}

void runProgram3() {
	//Teddy auton
	// Off starting line + 2 rings on Mobile Goal
	delayA();
	releaseGoal(); // Ensure clamp is open
	delayA(500);
	driveForTime(-100, -100, parseDistanceToTime(100, 40)); // Drive backward 40 centimeters
	delayA(500);
	seizeGoal(); // Clamp onto Mobile Goal
	delayA(500);
	scoreRing(); // Score pre-loaded ring
	delayA(1000);
	driveForTime((isRightCorner() ? -100 : 100), (isRightCorner() ? 100 : -100), parseAngleToTime(100, 75)); // Turn 75 degrees depending on corner
	delayA(500);
	driveAndIntakeForTime(100, 100, 90, 80, parseDistanceToTime(100, 50)); // Drive forward 50 centimeters while intaking
	delayA();
	scoreRing(); // Score bottom ring
	delayA();
}

void runProgram4() {
	//TEST BEFORE USE

	//drive backwards onto the goal spot, clamp goal, and score ring
	delayA();
	driveForTime(-127,-127,600);
	delayA();
	seizeGoal();
	delayA();
	scoreRing();
	delayA();

	//turn to face the 2 stack to the right, run onto it, and score ring
	
	driveForTime((isRightCorner() ? -100 : 100), (isRightCorner() ? 100 : -100), parseAngleToTime(100, 100));
	delayA();
	driveAndIntakeForTime(127,127,90,127,500);
	delayA();
	intakeForTime(90, 127, 3000);	
	delayA();

	//turn left, drive into a line stack, and score the ring
	driveForTime((isRightCorner() ? -100 : 100), (isRightCorner() ? 100 : -100), parseAngleToTime(100, -90));
	delayA();
	driveAndIntakeForTime(80,80,90,127,200); //make this more precise
	delayA();
	intakeForTime(90, 127, 3000);
	delayA();

	//drive back and touch ladder
	driveForTime(-127,-127,300);
	delayA();
	driveForTime((isRightCorner() ? -100 : 100), (isRightCorner() ? 100 : -100), parseAngleToTime(100, -50));
	delayA();
	motorsArm.move(127);
	delayA();
	driveForTime(127,127,420);
	delayA();
}

// Autonomous Helper Methods
void moveForTime(vector<void (*)(int n)> targets, vector<int> values, int milliseconds) {
	updateTargetsTo(targets, values);
	delay(milliseconds); // Wait
	updateTargetsTo(targets, vector<int>(values.size(), 0));
}

void updateTargetsTo(vector<void (*)(int n)> targets, vector<int> values) {
	// Resets all values
	for (int i = 0; i < targets.size() && i < values.size(); i++) {
		targets[i](values[i]);
	}
}

void driveForTime(int right, int left, int milliseconds) { moveForTime({&setTargetDriveLeft, &setTargetDriveRight}, {right, left}, milliseconds); }
void intakeForTime(int intake, int score, int milliseconds) { moveForTime({&setTargetIntake, &setTargetScore}, {intake, score}, milliseconds); }
void driveAndIntakeForTime(int right, int left, int intake, int score, int milliseconds) { moveForTime({&setTargetDriveRight, &setTargetDriveLeft, &setTargetIntake, &setTargetScore}, {right, left, intake, score}, milliseconds); }
void scoreRing() { intakeForTime(90, 80, 2500); }
void seizeGoal() { setTargetClamp(1); }
void releaseGoal() { setTargetClamp(0); }
void delayA(int milliseconds) { (milliseconds < autonDelay) ? delay(autonDelay) : delay(milliseconds); }

// Autonomous Parsing Methods
int parseDistanceToTime(int speed, int centimeters) { return (10 * centimeters + 60) / (abs(speed) * 0.01); }
int parseAngleToTime(int speed, int degrees) { return (4 * degrees + 40) / (abs(speed) * 0.01); }

// Autonomous Corner Methods
int getCurrentCorner() { return getCurrentSide() == 1 ? (getCurrentPosition() == 1 ? 1 : 2) : (getCurrentPosition() == 1 ? 3 : 4); }
bool isRedCorner() { return getCurrentSide() == 1; }
bool isBlueCorner() { return getCurrentSide() == 2; }
bool isPositiveCorner() { return getCurrentPosition() == 1; }
bool isNegativeCorner() { return getCurrentPosition() == 2; }
bool isRightCorner() { return getCurrentCorner() % 2 == 0; }
bool isLeftCorner() {return getCurrentCorner() % 2 != 0; }

/* Driver Controlled Period Task */
void controlInputsDriver() {

	setToggleBrake(false); // Updates brakes

	// Control Loop
	while (true) {

		// Update stored inputs from driver's controller
		updateInputsDriver();

		mutex.take(maxInterval); // Begin Exclusivity

		// Checks if the targets should be updated
		if (!getToggleManualAutonomous()) {

			// Update stored values
			updateToggles();
			updateTargets();
			updateActuals();
		}

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

// Driver Input Methods
void updateInputsDriver() {

	// Update stored inputs from controller
	inputsDriver.axis1 = masterController.get_analog(ANALOG_RIGHT_X);
	inputsDriver.axis2 = masterController.get_analog(ANALOG_RIGHT_Y);
	inputsDriver.axis3 = masterController.get_analog(ANALOG_LEFT_Y);
	inputsDriver.axis4 = masterController.get_analog(ANALOG_LEFT_X);
	inputsDriver.buttonX = masterController.get_digital(DIGITAL_X);
	inputsDriver.buttonA = masterController.get_digital(DIGITAL_A);
	inputsDriver.buttonB = masterController.get_digital(DIGITAL_B);
	inputsDriver.buttonY = masterController.get_digital(DIGITAL_Y);
	inputsDriver.buttonUp = masterController.get_digital(DIGITAL_UP);
	inputsDriver.buttonRight = masterController.get_digital(DIGITAL_RIGHT);
	inputsDriver.buttonDown = masterController.get_digital(DIGITAL_DOWN);
	inputsDriver.buttonLeft = masterController.get_digital(DIGITAL_LEFT);
	inputsDriver.buttonR1 = masterController.get_digital(DIGITAL_R1);
	inputsDriver.buttonR2 = masterController.get_digital(DIGITAL_R2);
	inputsDriver.buttonL1 = masterController.get_digital(DIGITAL_L1);
	inputsDriver.buttonL2 = masterController.get_digital(DIGITAL_L2);
}

void updateToggles() {

	// Calls helper methods
	updateTogglePID();
	updateToggleBrake();
	updateToggleKillSwitch();
}

void updateTogglePID() {
	static int aDelayCount{0};
	updateToggleTo(&setTogglePID, &getTogglePID, inputsDriver.buttonA, &aDelayCount);
}

void updateToggleBrake() {
	static int bDelayCount{0};
	updateToggleTo(&setToggleBrake, &getToggleBrake, inputsDriver.buttonB, &bDelayCount);
}

void updateToggleKillSwitch() {
	if (inputsDriver.buttonX == 1) {
		setToggleKillSwitch(true);
	}
	else {
		setToggleKillSwitch(false);
	}
}

void updateToggleTo(void (*set)(bool value), bool (*get)(void), int value, int *count) {

	// Checks if button is pressed, and hasn't been pressed for a delay
	if (value == 1 && *count >= doubleInputDelay) {

		// Flips the toggle
		set(!get());
		*count = 0;
	}

	// Increments the delay count
	*count++;
}

void updateTargets() {

	// Calls helper methods
	updateTargetDrivetrain();
	updateTargetArm();
	updateTargetIntakeScore();
	updateTargetClamp();
}

void updateTargetDrivetrain() {

	// Sets the targets to the inputs
	setTargetDriveLeft(inputsDriver.axis3);
	setTargetDriveRight(inputsDriver.axis2);
}

void updateTargetArm() {
	if (inputsDriver.buttonL1 == 1 && inputsDriver.buttonL2 == 0) {
		setTargetArm(90);
	}
	else if (inputsDriver.buttonL1 == 0 && inputsDriver.buttonL2 == 1) {
		setTargetArm(-90);
	}
	else {
		setTargetArm(0);
	}
}

void updateTargetIntakeScore() {
	if (inputsDriver.buttonR1 == 1 && inputsDriver.buttonR2 == 0) {
		setTargetIntake(90);
		setTargetScore(80);
	}
	else if (inputsDriver.buttonR1 == 0 && inputsDriver.buttonR2 == 1) {
		setTargetIntake(-90);
		setTargetScore(-80);
	}
	else {
		setTargetIntake(0);
		setTargetScore(0);
	}
}

void updateTargetClamp() {
	if (inputsDriver.buttonUp == 1 && inputsDriver.buttonDown == 0) {
		setTargetClamp(1);
	}
	else if (inputsDriver.buttonUp == 0 && inputsDriver.buttonDown == 1){
		setTargetClamp(0);
	}
}

/* Universal Period Task */
void controlInputsRobot() {

	// Control Loop
	while (true) {

		updateInputsRobot();

		mutex.take(maxInterval); // Begin Exclusivity

		updateActuals();

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

// Robot Input Methods
void updateInputsRobot() {
	inputsRobot.velocityR = motorsDriveRight.get_actual_velocity(0);
	inputsRobot.velocityL = motorsDriveLeft.get_actual_velocity(0);
}

void updateActuals() {
	updateActualDrivetrain();
}

void updateActualDrivetrain() {
	setActualDriveRight(inputsRobot.velocityR);
	setActualDriveLeft(inputsRobot.velocityL);
}

/* Robot System Task */
void controlRobot() {
	
	// Control loop
	while (true) {

		mutex.take(maxInterval); // Begin Exclusivity

		if (!getToggleKillSwitch()) {
			updateRobot();
		}

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

// Robot Update Methods
void updateRobot() {
	updateRobotDrivetrain();
	updateRobotArm();
	updateRobotIntakeScore();
	updateRobotClamp();
	updateRobotBrakes();
}

void updateRobotDrivetrain() {
	static int driveRightI{0};
	static int driveRightLastErorr{0};
	static int driveLeftI{0};
	static int driveLeftLastError{0};

	if (getTogglePID()) {
		motorsDriveRight.move(PID(getTargetDriveRight(), getActualDriveRight(), 0.6, 0.2, 0.2, &driveRightI, 10, &driveRightLastErorr));
		motorsDriveLeft.move(PID(getTargetDriveLeft(), getActualDriveLeft(), 0.6, 0.2, 0.2, &driveLeftI, 10, &driveLeftLastError));
	}
	else {
		motorsDriveRight.move(getTargetDriveRight());
		motorsDriveLeft.move(getTargetDriveLeft());
	}
}

void updateRobotArm() {
	motorsArm.move(getTargetArm());
}

void updateRobotIntakeScore() {
	motorIntake.move(getTargetIntake());
	motorScore.move(getTargetScore());
}

void updateRobotClamp() {
	if (getTargetClamp() == 1) {
		pneumaticClamp.extend();
	}
	else {
		pneumaticClamp.retract();
	}
}

void updateRobotBrakes() {
	motorsArm.set_brake_mode_all(MOTOR_BRAKE_HOLD);
	motorIntake.set_brake_mode_all(MOTOR_BRAKE_HOLD);
	motorScore.set_brake_mode_all(MOTOR_BRAKE_HOLD); 
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

/* Display System Task */
void controlDisplay() {

	// Control loop
	while (true) {

		if (currentState) { 
			updateScreens();
			updateGameInformation();
			updateDropdowns();
			updateImages();
			updateCanvas();
			updateTexts();
		}

		if (getCurrentPeriod() == 2 || getCurrentPeriod() == 3) {
			updateController();
		}

		delay(250); // Wait
	}
}

// Display Update Methods
void updateScreens() {
	int n = lv_dropdown_get_selected(dropdownHeader);

	if (n == getCurrentScreen()) {
		return;
	}
	else {
		setCurrentScreen(n);
		updateScreenTo(getScreenAt(n));
	}
}

void updateScreenTo(lv_obj_t *screen) {
	lv_scr_load(screen);

	for (int i = 0; i < screenObjects.size(); i++) {

		for (int j = 0; j < screenObjects[i].size(); j++) {
			if (i == 0) {
				lv_obj_set_parent(screenObjects[i][j], screen);
			}
			else {
				lv_obj_set_parent(screenObjects[i][j], getScreenAt(i - 1));
			}
		}
	}

	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
}

void updateGameInformation() {
	setCurrentSide(lv_dropdown_get_selected(dropdownAutonomousSide) + 1);
	setCurrentPosition(lv_dropdown_get_selected(dropdownAutonomousPosition) + 1);
	setCurrentAutonomous(lv_dropdown_get_selected(dropdownAutonomousAutonomous) + 1);
}

void updateImages() {
	switch (getCurrentMode()) {
		case 1:
			switch (getCurrentSide()) {
				case 1:
					updateImageTo(&imgAutonomousLayout, 295, 55, &dscImgLayoutVex, 0.64, true);
					break;
				case 2:
					updateImageTo(&imgAutonomousLayout, 295, 55, &dscImgLayoutVex, 0.64, false);
					break;
			}
			break;
		case 2:
			updateImageTo(&imgAutonomousLayout, 295, 55, &dscImgLayoutSkills, 0.64, false);
			break;
	}
}

void updateDropdowns() {
	int n = lv_dropdown_get_selected(dropdownAutonomousMode) + 1;

	if (n == getCurrentMode()) {
		return;
	}
	else {
		setCurrentMode(n);

		switch (getCurrentMode()) {
			case 1:
				updateDropdownOptionsTo(&dropdownAutonomousSide, "Red\nBlue");
				break;
			case 2:
				updateDropdownOptionsTo(&dropdownAutonomousSide, "Red");
				break;
		}
	}
}

void updateDropdownOptionsTo(lv_obj_t **dropdown, string options) {
	lv_dropdown_set_options(*dropdown, options.c_str());
}

void updateImageTo(lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom, bool flipped) {
	lv_img_set_src(*image, src);
	lv_img_set_pivot(*image, 0, 0);
	lv_img_set_zoom(*image, ((int) (256 * zoom)));
	if (flipped) {
		lv_img_set_angle(*image, 1800);
		lv_obj_align(*image, LV_ALIGN_TOP_LEFT, x + lv_obj_get_width(*image) * zoom, y + lv_obj_get_height(*image) * zoom);
	}
	else {
		lv_img_set_angle(*image, 0);
		lv_obj_align(*image, LV_ALIGN_TOP_LEFT, x, y);
	}
}

void updateCanvas() {
	lv_canvas_fill_bg(canvasHeader, colorCoolGray, LV_OPA_COVER);
	lv_canvas_draw_rect(canvasHeader, 0, 0, 480, 50, &dscRectHeader);
	lv_canvas_draw_rect(canvasHeader, 0, 50, 480, 230, &dscRectBody);
}

void updateTexts() {
	string empty = "";

	// Main Menu
    updateTextLabelTo(&textMainInfo, empty +
		"Mode: " + parseCurrentMode() + "\n" +
		"Period: " + parseCurrentPeriod() + "\n" +
		"Side: " + parseCurrentSide()  + "\n" +
		"Start Position: " + parseCurrentPosition() + "\n" +
		"Selected Autonomous: " + parseCurrentAutonomous());
	
	// Autonomous
	if (!getToggleManualAutonomous()) {
		updateTextLabelTo(&textAutonomousManual, empty +
			"Manual Autonomous");
	}
	else {
		updateTextLabelTo(&textAutonomousManual, empty +
			"Autonomous Running");
	}

	// Electronics
	updateTextTableTo(&tableElectronicsMotors,
		{40, 90, 50, 50}, {
		{"Port", "Description", "Targets", "Actuals"},
		{"1", "Right Drive", to_string(getTargetDriveRight()), to_string(getActualDriveRight())},
		{"2", "Right Drive", to_string(getTargetDriveRight()), to_string(getActualDriveRight())},
		{"-9", "Left Drive", to_string(getTargetDriveLeft()), to_string(getActualDriveLeft())},
		{"-10", "Left Drive", to_string(getTargetDriveLeft()), to_string(getActualDriveLeft())},
		{"7", "Intake", to_string(getTargetIntake()), ""}, 
		{"8", "Score", to_string(getTargetScore()), ""}, 
		{"", "", "", ""},
		{"", "", "", ""},
		{"", "", "", ""}});
	
	updateTextTableTo(&tableElectronicsADI,
		{40, 90, 50}, {
		{"Port", "Description", "Targets"},
		{"A", "Pneumatics", to_string(getTargetClamp())},
		{"B", "", ""},
		{"C", "", ""},
		{"D", "", ""},
		{"E", "", ""},
		{"F", "", ""},
		{"G", "", ""},
		{"H", "", ""},
		{"21", "Radio", ""}});
}

void updateTextLabelTo(lv_obj_t **label, string text) {
	lv_label_set_text(*label, text.c_str());
}

void updateTextTableTo(lv_obj_t **table, vector<int> widths, vector<vector<string>> text) {

	for (int i = 0; i < lv_table_get_col_cnt(*table) && i < widths.size(); i++) {
		lv_table_set_col_width(*table, i, widths[i]);
	}

	for (int r = 0; r < lv_table_get_row_cnt(*table) && r < text.size(); r++) {
		for (int c = 0; c < lv_table_get_col_cnt(*table) && c < text[0].size(); c++) {
			lv_table_set_cell_value(*table, r, c, text[r][c].c_str());
		}
	}	
}

void updateController() {
	masterController.print(0, 0, (to_string(getMaxTemp()) + "°C").c_str()); 
}

// Display Initializaiton Methods
void initializeAllGraphics() {
	initializeColors();
	initializeFonts();
	initializeDSCs();
	initializeStyles();
	initializeAllScreens();
	initializeText();
}

void initializeColors() {
	createColor(&colorWhite, 0xFFFFFF);
	createColor(&colorLightGray, 0xEFEFEF);
	createColor(&colorBlack, 0x000000);
	createColor(&colorDamienGreen, 0x244546);
	createColor(&colorSpartanGold, 0xFFB81C);
	createColor(&colorMilenniumGold, 0xB3A369);
	createColor(&colorCoolGray, 0xA7A8A9);
}

void initializeFonts() {
	LV_FONT_DECLARE(trajan_40);
	LV_FONT_DECLARE(fauna_14);
	LV_FONT_DECLARE(calibri_13);
	LV_FONT_DECLARE(calibri_11);
	
	createFont(&fontTrajan40, trajan_40);
	createFont(&fontFauna14, fauna_14);
	createFont(&fontCalibri13, calibri_13);
	createFont(&fontCalibri11, calibri_11);
}

void initializeDSCs() {
	LV_IMG_DECLARE(icon_spartan_helmet);
	LV_IMG_DECLARE(icon_spartan_helmet_gold);
	LV_IMG_DECLARE(layout_vex);
	LV_IMG_DECLARE(layout_skills);
	
	createDscImg(&dscImgSpartanHelmetGreen, icon_spartan_helmet);
	createDscImg(&dscImgSpartanHelmetGold, icon_spartan_helmet_gold);
	createDscImg(&dscImgLayoutVex, layout_vex);
	createDscImg(&dscImgLayoutSkills, layout_skills);

	createDscRect(&dscRectHeader, colorDamienGreen, 3, colorBlack);
	createDscRect(&dscRectBody, colorCoolGray, 3, colorBlack);
}

void initializeStyles() {
	createTextStyle(&styleTextHeader1, &fontTrajan40, colorSpartanGold);
	createTextStyle(&styleTextHeader2, &fontFauna14, colorSpartanGold);
	createTextStyle(&styleTextHeader3, &fontCalibri13, colorBlack);
	createButtonStyle(&styleButton1, &fontFauna14, colorSpartanGold, colorDamienGreen, colorBlack);
	createButtonStyle(&styleButton1Selected, &fontFauna14, colorWhite, colorSpartanGold, colorBlack);
	createButtonStyle(&styleButton2, &fontCalibri13, colorBlack, colorWhite, colorBlack);
	createButtonStyle(&styleButton2Selected, &fontCalibri13, colorSpartanGold, colorLightGray, colorBlack);
	createBoxStyle(&styleBox, &fontCalibri13, colorBlack, colorWhite, colorBlack);
	createTableStyle(&styleTableCell, &fontCalibri13, colorBlack, colorWhite, colorBlack);
}

void initializeAllScreens() {
	createScreen(&screenMainMenu);
	createScreen(&screenAutonomous);
	createScreen(&screenElectronics);
	createScreen(&screenInformation);

	lv_disp_load_scr(screenMainMenu);
	
	initializeHeader();
	initializeScreenMain();
	initializeScreenAutonomous();
	initializeScreenElectronics();
	initializeScreenInformation();

	updateScreenTo(screenMainMenu);
}

void initializeHeader() {
	createCanvas(-1, &canvasHeader, 0, 0, displayWidth, displayHeight);
	updateCanvas();
	createImage(-1, &imgHeader, 5, 5, &dscImgSpartanHelmetGold, 0.22);
	createLabel(-1, &textHeader1, 43, 0, &styleTextHeader1);
	createLabel(-1, &textHeader2, 185, 5, &styleTextHeader2);
	createDropdown(-1, &dropdownHeader, "Main Menu\nAutonomous\nElectronics\nInformation", 345, 5, 130, 40, true);

	screenObjects.push_back({canvasHeader, imgHeader, textHeader1, textHeader2, dropdownHeader});
}

void initializeScreenMain() {
	createBox(0, &buttonMainInfo, &textMainInfo, 10, 60);

	screenObjects.push_back({buttonMainInfo});
}

void initializeScreenAutonomous() {
	createDropdown(1, &dropdownAutonomousMode, "Competiton\nSkills", 10, 60, 110, 40, false);
	createDropdown(1, &dropdownAutonomousSide, "Red\nBlue", 10, 105, 110, 40, false);
	createDropdown(1, &dropdownAutonomousPosition, "Positive\nNegative", 10, 150, 110, 40, false);
	createDropdown(1, &dropdownAutonomousAutonomous, "Program #1\nProgram #2\nProgram #3\nProgram #4", 10, 195, 110, 40, false);
	createButton(1, &buttonAutonomousManual, &textAutonomousManual, 125, 60, 160, 40, eventAutonomousManual);
	createImage(1, &imgAutonomousLayout, 295, 55, &dscImgLayoutVex, 0.64);

	screenObjects.push_back({dropdownAutonomousMode, dropdownAutonomousSide, dropdownAutonomousPosition, dropdownAutonomousAutonomous, buttonAutonomousManual, imgAutonomousLayout});
}

void initializeScreenElectronics() {
	createTable(2, &tableElectronicsMotors, 10, 60, 10, 4);
	createTable(2, &tableElectronicsADI, 258, 60, 10, 3);

	screenObjects.push_back({tableElectronicsMotors, tableElectronicsADI});
}

void initializeScreenInformation() {
	createBox(3, &buttonInfoGame, &textInfoGame, 10, 60);
	createBox(3, &buttonInfoCoaches, &textInfoCoaches, 10, 110);
	createBox(3, &buttonInfoCode, &textInfoCode, 10, 176);
	createTable(3, &tableInfoTeam, 204, 60, 10, 3);

	screenObjects.push_back({buttonInfoGame, buttonInfoCoaches, buttonInfoCode, tableInfoTeam});
}

void initializeText() {
	string empty = "";

	// Header
	updateTextLabelTo(&textHeader1, empty +
		"6526-E");
	updateTextLabelTo(&textHeader2, empty +
		"E-is-for-Edward\nDamien Robotics");

	// Information
	updateTextLabelTo(&textInfoGame, empty +
		"VEX V5 Robotics Competition        \n" +
		"2024-2025 Game: High Stakes");
	updateTextLabelTo(&textInfoCoaches, empty +
		"Coaches: Ms. Maricic, Mr. Geiger,\n" +
		"Mr. Bikmaz, Mr. McElrea, Paige,\n" +
		"and others.");
	updateTextLabelTo(&textInfoCode, empty +
		"Programmed in C++ using the       \n" +
		"PROS framework and the LVGL\n" +
		"graphics library by Teddy.");

	updateTextTableTo(&tableInfoTeam,
		{95, 40, 120}, {
		{"Name", "Grade", "Role"},
		{"Edward Deng", "12", "Lead Builder"},
		{"Andres Gomez", "10", "Builder, Driver"},
		{"Aidan Reyes", "10", "Programmer, Builder"},
		{"Rylan Robles", "12", "Lead Driver, Builder"},
		{"Teddy Wachtler", "12", "Lead Programmer"},
		{"", "", ""},
		{"", "", ""},
		{"", "", ""},
		{"", "", ""}});
}

// Display Creation Methods
void createColor(lv_color_t *color, int hex) { *color = lv_color_hex(hex); }
void createFont(lv_font_t *font, lv_font_t source) { *font = source; }
void createDscImg(lv_img_dsc_t *dsc, lv_img_dsc_t source) { *dsc = source; }

void createDscRect(lv_draw_rect_dsc_t *dsc, lv_color_t bgColor, int borderWidth, lv_color_t borderColor) {
	lv_draw_rect_dsc_init(dsc);
    (*dsc).bg_color = bgColor;
	(*dsc).border_width = borderWidth;
    (*dsc).border_color = borderColor;
}

void createScreen(lv_obj_t **screen) { *screen = lv_obj_create(NULL); }

void createTextStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor) {
	lv_style_init(style);
	lv_style_set_text_font(style, textFont);
	lv_style_set_text_color(style, textColor);
}

void createButtonStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor) {
	lv_style_init(style); 
	lv_style_set_text_font(style, textFont);
	lv_style_set_text_color(style, textColor);
	lv_style_set_bg_color(style, bgColor);
	lv_style_set_radius(style, 0);
	lv_style_set_border_width(style, 2);
	lv_style_set_border_color(style, borderColor);
}

void createBoxStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor) {
	lv_style_init(style);
	lv_style_set_text_font(style, textFont);
	lv_style_set_text_color(style, textColor);
	lv_style_set_pad_all(style, 4);
	lv_style_set_bg_color(style, bgColor);
	lv_style_set_radius(style, 0);
	lv_style_set_border_width(style, 2);
	lv_style_set_border_color(style, borderColor);
}

void createTableStyle(lv_style_t *style, lv_font_t *textFont, lv_color_t textColor, lv_color_t bgColor, lv_color_t borderColor) {
	lv_style_init(style);
	lv_style_set_text_font(style, textFont);
	lv_style_set_text_color(style, textColor);
	lv_style_set_pad_all(style, 0.5);
	lv_style_set_bg_color(style, bgColor);
	lv_style_set_radius(style, 0);
	lv_style_set_border_width(style, 1);
	lv_style_set_border_color(style, borderColor);
}

void createCanvas(int screen, lv_obj_t **canvas, int x, int y, int width, int height) {
	static lv_color_t bufferAll[LV_CANVAS_BUF_SIZE_TRUE_COLOR(480, 240)];

	*canvas = lv_canvas_create(getScreenAt(screen));
	lv_obj_align(*canvas, LV_ALIGN_TOP_LEFT, x, y);
    lv_canvas_set_buffer(*canvas, bufferAll, width, height, LV_IMG_CF_TRUE_COLOR);
}

void createImage(int screen, lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom) {
	*image = lv_img_create(getScreenAt(screen));
	updateImageTo(image, x, y, src, zoom, false);
}

void createLabel(int screen, lv_obj_t **label, int x, int y, lv_style_t *style) {
	*label = lv_label_create(getScreenAt(screen));
    lv_obj_add_style(*label, style, 0);
    lv_obj_align(*label, LV_ALIGN_TOP_LEFT, x, y);
	lv_label_set_text(*label, "Text");
}

void createDropdown(int screen, lv_obj_t **dropdown, string options, int x, int y, int width, int height, bool header) {
	*dropdown = lv_dropdown_create(getScreenAt(screen));
	updateDropdownOptionsTo(dropdown, options);
	lv_dropdown_set_symbol(*dropdown, NULL);
	if (header) {
		lv_obj_add_style(*dropdown, &styleButton1, 0);
		lv_obj_add_style(lv_dropdown_get_list(*dropdown), &styleButton1, 0);
		lv_obj_add_style(lv_dropdown_get_list(*dropdown), &styleButton1Selected, LV_PART_SELECTED);
	}
	else {
		lv_obj_add_style(*dropdown, &styleButton2, 0);
		lv_obj_add_style(lv_dropdown_get_list(*dropdown), &styleButton2, 0);
		lv_obj_add_style(lv_dropdown_get_list(*dropdown), &styleButton2Selected, LV_PART_SELECTED);
	}
	lv_obj_set_size(*dropdown, width, height);
	lv_obj_align(*dropdown, LV_ALIGN_TOP_LEFT, x, y);
}

void createButton(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y, int width, int height, void(*function)(lv_event_t *event)) {
	*button = lv_btn_create(getScreenAt(screen));
	lv_obj_set_size(*button, width, height);
	lv_obj_align(*button, LV_ALIGN_TOP_LEFT, x, y);
	lv_obj_add_style(*button, &styleButton2, 0);
	*label = lv_label_create(*button);
	lv_obj_add_style(*label, &styleTextHeader3, 0);
	lv_obj_add_event_cb(*button, function, LV_EVENT_CLICKED, NULL);
}

void createBox(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y) {
	*button = lv_btn_create(getScreenAt(screen));
	lv_obj_align(*button, LV_ALIGN_TOP_LEFT, x, y);
	lv_obj_add_style(*button, &styleBox, 0);
	lv_obj_clear_flag(*button, LV_OBJ_FLAG_CLICKABLE);
	*label = lv_label_create(*button);
	lv_obj_add_style(*label, &styleTextHeader3, 0);
}

void createTable(int screen, lv_obj_t **table, int x, int y, int rows, int columns) {
	*table = lv_table_create(getScreenAt(screen));
	lv_obj_align(*table, LV_ALIGN_TOP_LEFT, x, y);
	lv_obj_add_style(*table, &styleBox, 0);
	lv_obj_add_style(*table, &styleTableCell, LV_PART_ITEMS);
	lv_table_set_row_cnt(*table, rows);
	lv_table_set_col_cnt(*table, columns);
	lv_obj_add_event_cb(*table, eventTableFormat, LV_EVENT_DRAW_PART_BEGIN, NULL); 
}

// Display Event Methods
void eventTableFormat(lv_event_t *event) {
    lv_obj_t *table = lv_event_get_target(event);
    lv_obj_draw_part_dsc_t *dscCell = lv_event_get_draw_part_dsc(event);
	lv_draw_rect_dsc_t *dscRectCell = (*dscCell).rect_dsc;

    if ((*dscCell).part == LV_PART_ITEMS) {
    	int r = (*dscCell).id /  lv_table_get_col_cnt(table);
    	int c = (*dscCell).id - r * lv_table_get_col_cnt(table);
    	
		if (r % 2 == 1) {
        	(*dscRectCell).bg_color = colorLightGray;
            (*dscRectCell).bg_opa = LV_OPA_COVER;
    	}
    }
}

void eventAutonomousManual(lv_event_t *event) {

	// Update brakes
	delayA();
	motorsDriveLeft.set_brake_mode_all(MOTOR_BRAKE_HOLD);
	motorsDriveRight.set_brake_mode_all(MOTOR_BRAKE_HOLD);

	// Manually triggers the autonomous program
	delayA();
	setToggleManualAutonomous(true);
	delayA(500);
	runAutonomous(); // Runs the program
	delayA(500);
	setToggleManualAutonomous(false);
	delayA();

	// Update brakes
	delayA();
	motorsDriveLeft.set_brake_mode_all(MOTOR_BRAKE_COAST);
	motorsDriveRight.set_brake_mode_all(MOTOR_BRAKE_COAST);
}

// Display Parsing Methods
string parseCurrentMode() {
	switch (getCurrentMode()) {
		case 1: return "Competition";
		case 2: return "Skills";
		default: return "Unknown";
	}
}

string parseCurrentPeriod() {
	switch (getCurrentPeriod()) {
		case -1: return "Disabled";
		case 1: return "Enabled";
		case 2: return "Driver Controlled";
		case 3: return "Autonomous Controlled";
		default: return "Unknown";
	}
}

string parseCurrentSide() {
	switch (getCurrentSide()) {
		case 1: return "Red      ";
		case 2: return "Blue     ";
		default: return "Unknown";
	}
}

string parseCurrentPosition() {
	switch (getCurrentPosition()) {
		case 1: return "Positive";
		case 2: return "Negative";
		default: return "Unknown";
	}
}

string parseCurrentAutonomous() {
	switch (getCurrentAutonomous()) {
		case 1: return "Program #1: Drive Backward";
		case 2: return "Program #2: 2-Ring Score";
		case 3: return "Program #3: [n/a]";
		case 4: return "Program #4: Testing";
		default: return "Unknown";
	}
}

string parseCurrentScreen() {
	switch (getCurrentScreen()) {
		case 0: return "Main Menu";
		case 1: return "Autonomous";
		case 2: return "Electronics";
		case 3: return "Information";
		default: return "Unknown";
	}
}

// Display Helper Methods
int getMaxTemp() {
	return max({
		motorsDriveLeft.get_temperature(0),
		motorsDriveLeft.get_temperature(1), 
		motorsDriveRight.get_temperature(0), 
		motorsDriveRight.get_temperature(1),
		motorsArm.get_temperature(0),
		motorsArm.get_temperature(1),
		motorScore.get_temperature(0), 
		motorIntake.get_temperature(0)
		});
}

lv_obj_t* getScreenAt(int screen) {
	switch (screen) {
		case -1:
			return lv_scr_act();
		case 0:
			return screenMainMenu;
		case 1:
			return screenAutonomous;
		case 2:
			return screenElectronics;
		case 3:
			return screenInformation;
		default:
			return screenMainMenu;
	}
}

/* Set Methods */
void setTogglePID(bool b) { togglePID = b; }
void setToggleBrake(bool b) { toggleBrake = b; }
void setToggleKillSwitch(bool b) { toggleKillSwitch = b; }
void setToggleManualAutonomous(bool b) { toggleManualAutonomous = b; }
void setActualDriveLeft(int n);
void setTargetDriveLeft(int n) { targetDriveLeft = n; }
void setTargetDriveRight(int n) { targetDriveRight = n; }
void setTargetArm(int n) { targetArm = n; }
void setTargetIntake(int n) { targetIntake = n; }
void setTargetScore(int n) { targetScore = n; }
void setTargetClamp(int n) { targetClamp = n; }
void setActualDriveLeft(int n) { actualDriveLeft = n; }
void setActualDriveRight(int n) { actualDriveRight = n; }
void setCurrentState(bool b) { currentState = b; }
void setCurrentMode(int n) { currentMode = n; }
void setCurrentPeriod(int n) { currentPeriod = n; }
void setCurrentSide(int n) { currentSide = n; }
void setCurrentPosition(int n) { currentPosition = n; }
void setCurrentAutonomous(int n) { currentAutonomous = n; }
void setCurrentScreen(int n) { currentScreen = n; }

/* Get Methods */
bool getTogglePID() { return togglePID; }
bool getToggleBrake() { return toggleBrake; }
bool getToggleKillSwitch() { return toggleKillSwitch; }
bool getToggleManualAutonomous() { return toggleManualAutonomous; }
int getTargetDriveLeft() { return targetDriveLeft; }
int getTargetDriveRight() { return targetDriveRight; }
int getTargetArm() { return targetArm; }
int getTargetIntake() { return targetIntake; }
int getTargetScore() { return targetScore; }
int getTargetClamp() { return targetClamp; }
int getActualDriveLeft() { return actualDriveLeft; }
int getActualDriveRight() { return actualDriveRight; }
bool getCurrentState() { return currentState; }
int getCurrentMode() { return currentMode; }
int getCurrentPeriod() { return currentPeriod; }
int getCurrentSide() { return currentSide; }
int getCurrentPosition() { return currentPosition; }
int getCurrentAutonomous() { return currentAutonomous; }
int getCurrentScreen() { return currentScreen; }
