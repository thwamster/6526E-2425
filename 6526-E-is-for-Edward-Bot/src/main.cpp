#include "main.h"
#include "liblvgl/lvgl.h"

using namespace std;
using namespace pros;

/* Global Objects */
// Constant Intervals
const int waitInterval = 12;
const int maxInterval = 100;
const int doubleInputDelay = 150;
const int autonDelay = 300;

// Screen Informaton
const int screenWidth = 480;
const int screenHeight = 240;
const int screenIndentTop = 100;
const int screenIndentRight = reverseScreenWidth(60);
const int screenIndentBottom = reverseScreenHeight(60);
const int screenIndentLeft = 60;

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

// Game Information
int currentPeriod{};
int currentSide{};
int currentPosition{};
int currentAutonomous{};
string displayText[9]{};

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

/* Competition */
void initialize() {

	// Update tasks
	autonomousPeriod.suspend();
	driverControlledPeriod.suspend();
	robotControlSystem.suspend();
	screenControlSystem.resume();

	updateGameInformation(); // Update game information

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

// Competition Helper Methods
void updateGameInformation() {
	drawScreenBackground();
	delay(250);
	setCurrentSide(drawScreenButtons("Select Side:", {"Red", "Blue"}, {Color::red, Color::blue}));
	delay(250);
	setCurrentPosition(drawScreenButtons("Select Position:", {"Positive", "Negative"}, {Color::yellow, Color::purple}));
	delay(250);
	setCurrentAutonomous(drawScreenButtons("Select Autonomous:", {"#1", "#2", "#3", "#4"}, {Color::red, Color::yellow, Color::green, Color::blue}));
	delay(250);
	drawScreenBackground();
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

	fill_n(displayText, (sizeof(displayText)/sizeof(displayText[0])), "");
	setDisplayTextAt(0, "6526 E-is-for-Edward Bot");
	drawScreenBackground();

	// Control loop
	while (true) {

		// If during driver controlled or autonomous periods
		if (getCurrentPeriod() == 2 || getCurrentPeriod() == 3) {

			// Update text to display
			updateDisplayText();

			// Display background
			if (bgDelay >= 100) {
				drawScreenBackground();
				bgDelay = 0;
			}
			bgDelay++;
			
			// Display foregrounds
			drawScreenHeader();
			drawScreenData();
			drawController();
		}

		delay(waitInterval); // Wait
	}
}

void updateDisplayText() {

	mutex.take(maxInterval); // Begin exclusivity
	
	// Updates lines from variables
	setDisplayTextAt(1, "Period: " + parseCurrentPeriod());
	setDisplayTextAt(2, "Side: " + parseCurrentSide() + ". Start Position: " + parseCurrentPeriod());
	setDisplayTextAt(3, "Selected Autonomous: " + parseCurrentAutonomous());
	setDisplayTextAt(4, "Right Target: " + to_string(rightDriveTarget) + ". Right Actual: " + to_string(rightDriveActual));
	setDisplayTextAt(5, "Left Target: " + to_string(leftDriveTarget) + ". Left Actual: " + to_string(leftDriveActual));
	setDisplayTextAt(6, "Intake Target: " + to_string(intakeScoreTarget) + ". Piston Target: " + to_string(pistonTarget));
	setDisplayTextAt(7, "PID enabled: " + to_string(enablePID));
		
	mutex.give(); // End exclusivity
}

void updateScreenColors(pros::Color pen, pros::Color eraser) {
	pros::screen::set_pen(pen);
	pros::screen::set_eraser(eraser);
}

// Screen Drawing Methods
void drawScreenBackground() {
	updateScreenColors(Color::white, Color::dark_green);
	pros::screen::erase_rect(0, 0, 480, 240);
}

void drawScreenButton(std::string text, int x1, int y1, int x2, int y2, pros::Color pen, pros::Color eraser) {
	updateScreenColors(pen, eraser);
	pros::screen::erase_rect(x1, y1, x2, y2);
	drawScreenText(text, (x1 + 20), (y1 + 20), pen, eraser);
}

void drawScreenText(std::string text, int x, int y, pros::Color pen, pros::Color eraser) {
	updateScreenColors(pen, eraser);
	pros::screen::print(E_TEXT_LARGE, x, y, text.c_str());
}

void drawScreenHeader() {
	updateScreenColors(Color::gold, Color::dark_green);
	pros::screen::print(pros::E_TEXT_LARGE_CENTER, 1, 10, getDisplayTextAt(0).c_str());
}

void drawScreenData() {
	updateScreenColors(Color::white, Color::dark_green);
	for (int i = 1; i < (sizeof(displayText)/sizeof(displayText[0])); i++) {
		pros::screen::print(pros::E_TEXT_MEDIUM, 5, ((i + 1) * 22), getDisplayTextAt(i).c_str());
	}
}

void drawController() {
	masterController.print(0, 0, ("6526-E: " + parseCurrentSide()).c_str());
}

int drawScreenButtons(std::string prompt, std::vector<std::string> icons, std::vector<pros::Color> colors) {
	int n = std::min(icons.size(), colors.size());
	int width = (screenIndentRight - screenIndentLeft) / n;

	drawScreenBackground();
	drawScreenText(prompt, screenIndentLeft, screenIndentLeft, Color::white, Color::dark_green);
	for (int i = 0; i < n; i++) {
		drawScreenButton(icons[i], (screenIndentLeft + (width * i)), screenIndentTop, (screenIndentLeft + (width * (i + 1))), screenIndentBottom, Color::white, colors[i]);
	}
	drawScreenHeader();

	delay(1000);

	return waitUntilButtonPressed(n, width);
}

int waitUntilButtonPressed(int n, int width) {
	while (pros::screen::touch_status().touch_status != pros::E_TOUCH_HELD) {
		delay(waitInterval);
	}

	int x = pros::screen::touch_status().x;
	int y = pros::screen::touch_status().y;

	if (!isBetween(y, screenIndentTop, screenIndentBottom)) {
		return waitUntilButtonPressed(n, width);
	}

	for (int i = 0; i < n; i++) {
		if (isBetween(x, (screenIndentLeft + (width * i)), (screenIndentLeft + (width * (i + 1))))) {
			return (i + 1);
		}
	}

	return waitUntilButtonPressed(n, width);
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

// Screen Helpr Methods
int reverseScreenWidth(int n) { return screenWidth - n; }
int reverseScreenHeight(int n) { return screenHeight - n; }
bool isBetween(int n, int min, int max) { return n >= min && n <= max; }

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
void setDisplayTextAt(int i, std::string s) { displayText[i] = s; }

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
std::string getDisplayTextAt(int i) { return displayText[i]; }

/* LVGL */
void foo() {
	lv_obj_t* headerLabel;

	lv_label_create(headerLabel);

	delay(10000);
}
