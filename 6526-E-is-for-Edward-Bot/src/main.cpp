#include "main.h"

/* Competition Methods */
void initialize() {

	// Updsate graphics
	initializeAllGraphics();
	currentState = true;

	// Update tasks
	autonomousPeriod.suspend();
	driverControlledPeriod.suspend();
	robotControlSystem.suspend();
	displayControlSystem.resume();

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
void moveForTime(vector<int*> targets, vector<int> values, int t) {

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

/* Display Control System Task */
void updateDisplay() {

	// Control loop
	while (true) {

		if (currentState) { 
			updateScreen();
			updateGameInformation();
			updateImages();
			updateCanvas();
			updateText();
		}

		if (getCurrentPeriod() == 2 || getCurrentPeriod() == 3) {
			updateController();
		}

		delay(250); // Wait
	}
}

void updateScreen() {
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
	setCurrentMode(lv_dropdown_get_selected(dropdownAutonomousMode) + 1);
	setCurrentSide(lv_dropdown_get_selected(dropdownAutonomousSide) + 1);
	setCurrentPosition(lv_dropdown_get_selected(dropdownAutonomousPosition) + 1);
	setCurrentAutonomous(lv_dropdown_get_selected(dropdownAutonomousAutonomous) + 1);
}

void updateImages() {
	int n = lv_dropdown_get_selected(dropdownAutonomousMode) + 1;

	if (n == getCurrentMode()) {
		return;
	}
	else {
		setCurrentMode(n);

		switch (n) {
			case 1: 
				updateImageTo(&imgMainLayout, 295, 55, &dscImgLayoutVex, 0.64);
				break;
			case 2: 
				updateImageTo(&imgMainLayout, 295, 55, &dscImgLayoutSkills, 0.64);
				break;
		}
	}
}

void updateImageTo(lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom) {
	lv_img_set_src(*image, src);
	lv_img_set_pivot(*image, 0, 0);
	lv_img_set_zoom(*image, ((int) (256 * zoom)));
	lv_obj_align(*image, LV_ALIGN_TOP_LEFT, x, y);
}

void updateCanvas() {
	lv_canvas_fill_bg(canvasHeader, colorCoolGray, LV_OPA_COVER);
	lv_canvas_draw_rect(canvasHeader, 0, 0, 480, 50, &dscRectHeader);
	lv_canvas_draw_rect(canvasHeader, 0, 50, 480, 230, &dscRectBody);
}

void updateText() {
	// Header
	lv_label_set_text(textHeader1, "6526-E");
	lv_label_set_text(textHeader2, "E-is-for-Edward\nDamien Robotics");
	
	// Main Menu
	string stringMainInfo = "Mode: " + parseCurrentMode() + "\nPeriod: " + parseCurrentPeriod() + "\nSide: " + parseCurrentSide()  + "\nStart Position: " + parseCurrentPosition() + "\nSelected Autonomous: " + parseCurrentAutonomous();
    lv_label_set_text(textMainInfo, stringMainInfo.c_str());
	
	// Electronics	
	string arrayElectronicsMotors[10][4] = {
		{"Port", "Description", "Targets", "Actuals"},
		{"1", "Right Drive", to_string(rightDriveTarget), to_string(rightDriveActual)},
		{"2", "Right Drive", to_string(rightDriveTarget), to_string(rightDriveActual)},
		{"-9", "Left Drive", to_string(leftDriveTarget), to_string(leftDriveActual)},
		{"-10", "Left Drive", to_string(leftDriveTarget), to_string(leftDriveActual)},
		{"7", "Intake-Score", to_string(intakeScoreTarget), ""}, 
		{"8", "Intake-Score", to_string(intakeScoreTarget), ""}, 
		{"", "", "", ""},
		{"", "", "", ""},
		{"", "", "", ""}};

	lv_table_set_col_width(tableElectronicsMotors, 0, 40);
	lv_table_set_col_width(tableElectronicsMotors, 1, 90);
	lv_table_set_col_width(tableElectronicsMotors, 2, 50);
	lv_table_set_col_width(tableElectronicsMotors, 3, 50);

	for (int r = 0; r < lv_table_get_row_cnt(tableElectronicsMotors); r++) {
		for (int c = 0; c < lv_table_get_col_cnt(tableElectronicsMotors); c++) {
			lv_table_set_cell_value(tableElectronicsMotors, r, c, arrayElectronicsMotors[r][c].c_str());
		}
	}

	string arrayElectronicsADI[10][3] = {
		{"Port", "Description", "Targets"},
		{"A", "Pneumatics", to_string(pistonTarget)},
		{"B", "", ""},
		{"C", "", ""},
		{"D", "", ""},
		{"E", "", ""},
		{"F", "", ""},
		{"G", "", ""},
		{"H", "", ""},
		{"21", "Radio", ""}};

	lv_table_set_col_width(tableElectronicsADI, 0, 40);
	lv_table_set_col_width(tableElectronicsADI, 1, 90);
	lv_table_set_col_width(tableElectronicsADI, 2, 50);

	for (int r = 0; r < lv_table_get_row_cnt(tableElectronicsADI); r++) {
		for (int c = 0; c < lv_table_get_col_cnt(tableElectronicsADI); c++) {
			lv_table_set_cell_value(tableElectronicsADI, r, c, arrayElectronicsADI[r][c].c_str());
		}
	}

	// Information
	lv_label_set_text(textInfoGame, "VEX V5 Robotics Competition        \n2024-2025 Game: High Stakes");
	lv_label_set_text(textInfoCoaches, "Coaches: Ms. Maricic, Mr. Geiger,\nMr. Bikmaz, Mr. McElrea, Paige,\nand others.");
	lv_label_set_text(textInfoCode, "Programmed in C++ using the       \nPROS framework and the LVGL\ngraphics library by Teddy.");

	string arrayInfoTeam[10][3] = {
		{"Name", "Grade", "Role"},
		{"Edward Deng", "12", "Lead Builder"},
		{"Andres Gomez", "10", "Builder, Driver"},
		{"Aidan Reyes", "10", "Programmer, Builder"},
		{"Rylan Robles", "12", "Lead Driver, Builder"},
		{"Teddy Wachtler", "12", "Lead Programmer"},
		{"", "", ""},
		{"", "", ""},
		{"", "", ""},
		{"", "", ""}};

	lv_table_set_col_width(tableInfoTeam, 0, 95);
	lv_table_set_col_width(tableInfoTeam, 1, 40);
	lv_table_set_col_width(tableInfoTeam, 2, 120);

	for (int r = 0; r < lv_table_get_row_cnt(tableInfoTeam); r++) {
		for (int c = 0; c < lv_table_get_col_cnt(tableInfoTeam); c++) {
			lv_table_set_cell_value(tableInfoTeam, r, c, arrayInfoTeam[r][c].c_str());
		}
	}	
}

void updateController() {
	masterController.print(0, 0, ("6526-E: " + parseCurrentSide()).c_str());
}

// Display Initializaiton Methods
void initializeAllGraphics() {
	initializeColors();
	initializeFonts();
	initializeDSCs();
	initializeStyles();
	initializeAllScreens();
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
	createImage(0, &imgMainLayout, 295, 55, &dscImgLayoutVex, 0.64);

	screenObjects.push_back({buttonMainInfo, imgMainLayout});
}

void initializeScreenAutonomous() {
	createDropdown(1, &dropdownAutonomousMode, "Competiton\nSkills", 10, 60, 110, 40, false);
	createDropdown(1, &dropdownAutonomousSide, "Red\nBlue", 125, 60, 110, 40, false);
	createDropdown(1, &dropdownAutonomousPosition, "Positive\nNegative", 240, 60, 110, 40, false);
	createDropdown(1, &dropdownAutonomousAutonomous, "Program #1\nProgram #2\nProgram #3\nProgram #4", 355, 60, 110, 40, false);

	screenObjects.push_back({dropdownAutonomousMode, dropdownAutonomousSide, dropdownAutonomousPosition, dropdownAutonomousAutonomous});
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

void createCanvas(int screen, lv_obj_t **canvas, int x, int y, int w, int h) {
	*canvas = lv_canvas_create(getScreenAt(screen));
	lv_obj_align(*canvas, LV_ALIGN_TOP_LEFT, x, y);
    lv_canvas_set_buffer(*canvas, bufferAll, w, h, LV_IMG_CF_TRUE_COLOR);
}

void createImage(int screen, lv_obj_t **image, int x, int y, lv_img_dsc_t *src, double zoom) {
	*image = lv_img_create(getScreenAt(screen));
	updateImageTo(image, x, y, src, zoom);
}

void createLabel(int screen, lv_obj_t **label, int x, int y, lv_style_t *style) {
	*label = lv_label_create(getScreenAt(screen));
    lv_obj_add_style(*label, style, 0);
    lv_obj_align(*label, LV_ALIGN_TOP_LEFT, x, y);
	lv_label_set_text(*label, "Text");
}

void createDropdown(int screen, lv_obj_t **dropdown, string options, int x, int y, int w, int h, bool header) {
	*dropdown = lv_dropdown_create(getScreenAt(screen));
	lv_dropdown_set_options(*dropdown, options.c_str());
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
	lv_obj_set_size(*dropdown, w, h);
	lv_obj_align(*dropdown, LV_ALIGN_TOP_LEFT, x, y);
}

void createBox(int screen, lv_obj_t **button, lv_obj_t **label, int x, int y) {
	*button = lv_btn_create(getScreenAt(screen));
	lv_obj_align(*button, LV_ALIGN_TOP_LEFT, x, y);
	lv_obj_add_style(*button, &styleBox, 0);
	lv_obj_clear_flag(*button, LV_OBJ_FLAG_CLICKABLE);
	*label = lv_label_create(*button);
	lv_obj_add_style(*label, &styleTextHeader3, 0);
}

void createTable(int screen, lv_obj_t **table, int x, int y, int r, int c) {
	*table = lv_table_create(getScreenAt(screen));
	lv_obj_align(*table, LV_ALIGN_TOP_LEFT, x, y);
	lv_obj_add_style(*table, &styleBox, 0);
	lv_obj_add_style(*table, &styleTableCell, LV_PART_ITEMS);
	lv_table_set_row_cnt(*table, r);
	lv_table_set_col_cnt(*table, c);
	lv_obj_add_event_cb(*table, createTableFormat, LV_EVENT_DRAW_PART_BEGIN, NULL); 
}

void createTableFormat(lv_event_t *e) {
    lv_obj_t *table = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);

    if(dsc->part == LV_PART_ITEMS) {
    	int r = dsc->id /  lv_table_get_col_cnt(table);
    	int c = dsc->id - r * lv_table_get_col_cnt(table);
    	
		if (r % 2 == 1) {
        	dsc->rect_dsc->bg_color = colorLightGray;
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
    	}
    }
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
		case 1: return "Initializing";
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
		case 1: return "Program #1: Preload Score";
		case 2: return "Program #2";
		case 3: return "Program #3";
		case 4: return "Program #4";
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
lv_obj_t* getScreenAt(int n) {
	switch (n) {
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
void setLeftDriveTarget(int n) { leftDriveTarget = n; }
void setRightDriveTarget(int n) { rightDriveTarget = n; }
void setIntakeScoreTarget(int n) { intakeScoreTarget = n; }
void setPistonTarget(int n) { pistonTarget = n; }
void setEnablePID(bool b) { enablePID = b; }
void setLeftDriveActual(int n) { leftDriveActual = n; }
void setRightDriveActual(int n) { rightDriveActual = n; }
void setCurrentState(bool b) { currentState = b; }
void setCurrentMode(int n) { currentMode = n; }
void setCurrentPeriod(int n) { currentPeriod = n; }
void setCurrentSide(int n) { currentSide = n; }
void setCurrentPosition(int n) { currentPosition = n; }
void setCurrentAutonomous(int n) { currentAutonomous = n; }
void setCurrentScreen(int n) { currentScreen = n; }

/* Get Methods */
int getLeftDriveTarget() { return leftDriveTarget; }
int getRightDriveTarget() { return rightDriveTarget; }
int getIntakeScoreTarget() { return intakeScoreTarget; }
int getPistonTarget() { return pistonTarget; }
int getLeftDriveActual() { return leftDriveActual; }
int getRightDriveActual() { return rightDriveActual; }
bool getEnablePID() { return enablePID; }
bool getCurrentState() { return currentState; }
int getCurrentMode() { return currentMode; }
int getCurrentPeriod() { return currentPeriod; }
int getCurrentSide() { return currentSide; }
int getCurrentPosition() { return currentPosition; }
int getCurrentAutonomous() { return currentAutonomous; }
int getCurrentScreen() { return currentScreen; }