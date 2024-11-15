#include "main.h"
using namespace std;
using namespace pros;

/* Global Objects */
// Constant Intervals
const int waitInterval = 12;
const int maxInterval = 100;
const int doubleInputDelay = 150;

// Tasks
pros::Mutex mutex;
pros::Task driverControlledPeriod(getDriverInputs);
pros::Task autonomousPeriod(runAutonomousInputs);
pros::Task robotControlSystem(updateRobot);
pros::Task debug(updateDisplay);

// Controller
pros::Controller masterController(E_CONTROLLER_MASTER);

// Robot Movement
pros::MotorGroup leftDriveMotors({-9, -10});
pros::MotorGroup rightDriveMotors({1,2});
pros::MotorGroup intakeMotor({7});
pros::MotorGroup scoreMotor({8});
adi::Pneumatics clampPistons (1, true);

// Robot Sensors
// TODO: Implement

// Target Variables
int leftDriveTarget{};
int rightDriveTarget{};
int intakeScoreTarget{};
bool pistonTarget{false};

// Actual Variables
int leftDriveActual{};
int rightDriveActual{};

// Toggles
bool enablePID{false};

// Debug
string displayText[10]{};

/* Competition Methods */

/* 
 * Initialization method. Called by the competition.
 */
void initialize() {

	// Initialize LCD
	lcd::initialize();
	lcd::register_btn1_cb(on_center_button);

	resetAllVariables(); // Resets variables
	displayClear(); // Clears display

	// Update tasks
	driverControlledPeriod.suspend();
	autonomousPeriod.suspend();
	robotControlSystem.suspend();
	debug.resume();
}

/* 
 * Disabled state. Called by the competition. Unused.
 */
void disabled() {}	

/* 
 * Competition initialization method. Called by the competition. Unused.
 */
void competition_initialize() {}

/* 
 * Autonomous control method. Called by the competition.
 */
void autonomous() {

	resetAllVariables(); // Resets variables
	displayClear(); // Clears display
	
	// Update tasks
	driverControlledPeriod.suspend();
	autonomousPeriod.resume();
	robotControlSystem.resume();

	// Control loop
	while (true) {
		
		// Update display
		displayText[2] = "Under autonomous control.";

		// Wait
		delay(waitInterval);
	}
}

/* 
 * Operator control method. Called by the competition.
 */
void opcontrol() {

	resetAllVariables(); // Resets variables
	displayClear(); // Clears display

	// Update Tasks
	driverControlledPeriod.resume();
	autonomousPeriod.suspend();
	robotControlSystem.resume();

	// Control loop
	while (true) {
		
		// Update display
		displayText[3] = "Under operator control.";

		delay(waitInterval); // Wait
	}
}

/* Autonomous Period Task */

/*
 * Autonomous method. Runs the selected autonomous program, and called by the task.
 */
void runAutonomousInputs() {

	// Runs autonomous program
	switch (selectAutonomous()) {

		// Program #1: 
		case 1:
			// moveForTime(vector<MotorGroup*>(&leftDriveMotors, &rightDriveMotors), vector<int>{127, 127}, 100);
			break;
			
		// Program #2:
		case 2:
			break;
		
		// Program #3:
		case 3:
			break;
	}

	resetAllVariables(); // Resets

	// Update tasks
	autonomousPeriod.suspend();
}

/*
 * Autonomous method. Selects the correct autonomous program.
 */
int selectAutonomous() {
	int n{};

	// TODO: Implement selector
	n = 1;

	return n; // Returns
}

/*
 * Autonomous method. Moves a list of motors to specificed values for a particular interval of time.
 */
void moveForTime(std::vector<pros::MotorGroup*> motors, std::vector<int> values, int t) {

	// Time loop
	for (int s = 0; s < t; s += waitInterval) {

		// Moves all motors
		for (int i = 0; i < sizeof(motors); i++) {
			(*motors[i]).move(values[i]);
		}

		delay(waitInterval); // Wait
	}

	// Resets all motors
	for (int i = 0; i < sizeof(motors); i++) {
		(*motors[i]).move(0);
	}
}

/* Driver Controlled Period Task */

/* 
 * Receives inputs from controllers and sensors and updates global variables.
 */
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
		leftDriveTarget = axis3;
		rightDriveTarget = axis2;

		if (buttonA == 1 && aDelayCount >= doubleInputDelay) {
			enablePID = !enablePID;
			aDelayCount = 0;
		}
		aDelayCount++;
		
		if (buttonUp == 1 && buttonDown == 0) {
			pistonTarget = true;
		}
		else if (buttonUp == 0 && buttonDown == 1){
			pistonTarget = false;
		}

		if (buttonR1 == 1 && buttonR2 == 0) {
			intakeScoreTarget = 1;
		}
		else if (buttonR1 == 0 && buttonR2 == 1) {
			intakeScoreTarget = -1;
		}
		else {
			intakeScoreTarget = 0;
		}

		mutex.give(); // End Exclusivity

		// Sets actuals
		rightDriveActual = velocityR;
		leftDriveActual = velocityL;

		delay(waitInterval); // Wait
	}
}

/* Robot Control System Task */

/* 
 * Driver control system. Updates motors and electronics from target variables.
 */
void updateRobot() {
	int rightDriveI = 0;
	int rightDriveLastError = 0;
	int leftDriveI = 0;
	int leftDriveLastError = 0;

	// Control loop
	while (true) {

		// Update display
		displayText[4] = "Updating robot.";

		mutex.take(maxInterval); // Begin Exclusivity

		if (enablePID) {
			rightDriveMotors.move(PID(rightDriveTarget, rightDriveActual, 0.6, 0.2, 0.2, &rightDriveI, 10, &rightDriveLastError));
			leftDriveMotors.move(PID(leftDriveTarget, leftDriveActual, 0.6, 0.2, 0.2, &leftDriveI, 10, &leftDriveLastError));
		}
		else {
			rightDriveMotors.move(rightDriveTarget);
			leftDriveMotors.move(leftDriveTarget);
		}

		if (pistonTarget) {
			clampPistons.extend();
		}
		else {
			clampPistons.retract();
		}

		if (intakeScoreTarget == 1) {
			intakeMotor.move(90);
			scoreMotor.move(80);
		}
		else if (intakeScoreTarget == 2) {
			intakeMotor.move(-90);
			scoreMotor.move(-80);
			
		}
		else if (intakeScoreTarget == 0) {
			intakeMotor.move(0);
			scoreMotor.move(0);
		}

		mutex.give(); // End Exclusivity

		delay(waitInterval); // Wait
	}
}

/* 
 * PID control system method.
 */
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

/* Debug Task */

/* 
 * Debug method. Updates the LCD on the brain.
 */
void updateDisplay() {

	displayClear(); // Clears display

	// Control loop
	while (true) {

		mutex.take(maxInterval); // Begin exclusivity
	
		// Updates lines from variables
		displayText[5] = "Right Target: " + to_string(rightDriveTarget) + ". Right Actual: " + to_string(rightDriveActual);
		displayText[6] = "Left Target: " + to_string(leftDriveTarget) + ". Left Actual: " + to_string(leftDriveActual);
		displayText[7] = "Intake Target: " + to_string(intakeScoreTarget);
		displayText[8] = "Pnemautic Target: " + to_string(pistonTarget);
		displayText[9] = "PID enabled: " + to_string(enablePID);
		
		mutex.give(); // End exclusivity

		// Loop through all lines
		for (int i = 0; i < sizeof(displayText); i++) {

			// Update LCD
			lcd::set_text(i, displayText[i]);
		}

		delay(waitInterval); // Wait
	}
}

/*
 * Debug method. Clears the array of strings that displays on the screen.
 */
void displayClear() {

	// Clears display
	fill_n(displayText, sizeof(displayText), "");

	// Assigns initialization text
	displayText[1] = "~~~ 6526-E-is-for-Edward-Bot ~~~";
}

/*
 * Reset method. Resets all instance variables.
 */
void resetAllVariables() {

	// Resets targets
	leftDriveTarget = 0;
	rightDriveTarget = 0;
	intakeScoreTarget = 0;
	pistonTarget = false;

	// Resets actuals
	leftDriveActual = 0;
	rightDriveActual = 0;
	
	// Resets toggles
	enablePID = false;
}

/* 
 * Center Button reaction method. Unused.
 */
void on_center_button() {}

/* 
 * Electronics
 *
 * Drivetrain: 
 * 	  Left front: Port 10
 * 	  Left back: Port 9
 * 	  Right front: Port 2
 * 	  Right back: Port 1
 * Intake-Score:
 * 	  Intake: Port 7 
 * 	  Score: Port 8
 * Inertial Sensors:
 * 	  Left side: Port 2
 * 	  Right side: Port 1
 * Radio: Port 21
 * Clamp Pistons: 3 Wire Ouput A
 */