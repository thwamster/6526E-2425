#include "main.h"

/* Global Objects */
// Constant Intervals
const int waitInterval = 10;
const int maxInterval = 100;

// Tasks
pros::Mutex mutex;

// Electronics
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::MotorGroup leftDriveMotors({-9, -10});
pros::MotorGroup rightDriveMotors({1, 2});
pros::adi::Pneumatics backPistons(1, true);

// Target & Sensor Variables
int leftDriveTarget;
int rightDriveTarget;
bool pistonTarget;
bool enablePID;

int leftInertialSensor;
int rightInertialSensor;

/* Competition Methods */
/* 
 * TODO
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/* 
 * TODO
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Initialized.");

	mutex.take(maxInterval); // Begin Exclusivity

	// Initialize Variables
	leftDriveTarget = 0;
	rightDriveTarget = 0;
	pistonTarget = false;
	enablePID = false;

	leftInertialSensor = 0;
	rightInertialSensor = 0;

	// Initialize Tasks
	mutex.give(); // End Exclusivity

	
	pros::lcd::register_btn1_cb(on_center_button);
}

/* 
 * TODO
 */
void disabled() {
}

/* 
 * TODO
 */
void competition_initialize() {}

/* 
 * TODO
 */
void autonomous() {}

/* 
 * TODO
 */
void opcontrol() {
	pros::Task inputs(getInputs);
	pros::Task drive(driveControlSystem);

	while (true) {
		pros::lcd::set_text(1, "Under operator control.");

		// Wait
		pros::delay(waitInterval);
	}
}

/* Control Methods */
/* 
 * TODO
 */
void getInputs() {
	const int doubleInputDelay = 200;
	int aDelayCount = 0;

	int leftStick = 0;
	int rightStick = 0;
	int upButton = 0;
	int downButton = 0;
	int aButton = 0;

	// Input and Electronics Loop
	while (true) {
		pros::lcd::set_text(2, "Electronics updating.");

		// Receives Inputs
		leftStick = master.get_analog(ANALOG_LEFT_Y);
		rightStick = master.get_analog(ANALOG_RIGHT_Y);
		upButton = master.get_digital(DIGITAL_UP);
		downButton = master.get_digital(DIGITAL_DOWN);
		aButton = master.get_digital(DIGITAL_A);

		// Receives Sensors
		leftInertialSensor = 0; // TODO
		rightInertialSensor = 0; // TODO

		// Sets Targets
		mutex.take(maxInterval); // Begin Exclusivity

		leftDriveTarget = leftStick;
		rightDriveTarget = rightStick;

		if (upButton == 1 && downButton == 0) {
			pistonTarget = true;
			pros::lcd::set_text(4, "Pneumatics enabled.");
		}
		else if (upButton == 0 && downButton == 1) {
			pistonTarget = false;
			pros::lcd::set_text(4, "Pneumatics disabled.");
		}

		if (aButton == 1 && aDelayCount >= doubleInputDelay) {
			enablePID = !enablePID;
			aDelayCount = 0;
		}

		mutex.give(); // End Exclusivity

		// Wait
		pros::delay(waitInterval);

		aDelayCount++;
	}
}

/* 
 * TODO
 */
void driveControlSystem() {
	int rightDriveI = 0;
	int rightDriveLastError = 0;

	int leftDriveI = 0;
	int leftDriveLastError = 0;

	// Drive Control System Loop
	while (true) {
		pros::lcd::set_text(3, "Control system processing.");

		// Sets Electronics
		mutex.take(maxInterval); // Begin Exclusivity
		
		if (enablePID) {
			pros::lcd::set_text(5, "PID enabled.");
			rightDriveMotors.move(PID(rightDriveTarget, rightInertialSensor, 1, 0, 0, &rightDriveI, 10, &rightDriveLastError));
			leftDriveMotors.move(PID(leftDriveTarget, leftInertialSensor, 1, 0, 0, &leftDriveI, 10, &leftDriveLastError));
		}
		else {
			pros::lcd::set_text(5, "PID disabled.");
			rightDriveMotors.move(rightDriveTarget);
			leftDriveMotors.move(leftDriveTarget);
		}

		if (pistonTarget) {
			backPistons.extend();
		}
		else {
			backPistons.retract();
		}

		mutex.give(); // End Exclusivity

		// Wait
		pros::delay(waitInterval);
	}
}

/* 
 * TODO
 */
int PID(int target, int sensor, int kP, int kI, int kD, int* I, int limitI, int* lastError) {
	int PID = 0;
	int error = target - sensor;

	if (abs(error) < limitI) {
		*I += error;
	}
	else {
		*I = 0;
	}

	PID = (kP * error) + (kI + *I) + (kD * error - *lastError);
	*lastError = error;

	return PID;
}	