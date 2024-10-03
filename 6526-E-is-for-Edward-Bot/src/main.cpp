#include "main.h"

/* Global Objects */
// Constant Intervals
const int waitInterval = 10;
const int maxInterval = 100;

// Tasks
pros::Mutex mutex;
pros::Task inputs;
pros::Task drive;

// Electronics
pros::Controller master;
pros::MotorGroup leftDriveMotors({-2, -9});
pros::MotorGroup rightDriveMotors({1, 10});
pros::ADIDigitalOut backPistons('A');

// Target & Sensor Variables
int leftDriveTarget;
int rightDriveTarget;
bool pistonTarget;

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
	pros::lcd::register_btn1_cb(on_center_button);

	mutex.take(maxInterval); // Begin Exclusivity

	// Initialize Variables
	leftDriveTarget = 0;
	rightDriveTarget = 0;
	pistonTarget = false;

	leftInertialSensor = 0;
	rightInertialSensor = 0;

	// Initialize Tasks
	inputs = pros::Task::create(opcontrol);
	inputs.suspend();
	drive.suspend();
	inputs = pros::Task::create(driveControlSystem);

	mutex.give(); // End Exclusivity
}

/* 
 * TODO
 */
void disabled() {
	inputs.suspend();
	drive.suspend();
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
	inputs.resume();
	drive.resume();

	while (true) {
		pros::lcd::set_text(1, "Under operator control.");
	}
}

/* Control Methods */
/* 
 * TODO
 */
void getInputs() {
	int leftStick = 0;
	int rightStick = 0;
	int upButton = 0;
	int downButton = 0;

	// Operator Control Loop
	while (true) {

		// Receives Inputs
		leftStick = master.get_analog(ANALOG_LEFT_Y);
		rightStick = master.get_analog(ANALOG_RIGHT_Y);
		upButton = master.get_digital(DIGITAL_UP);
		downButton = master.get_digital(DIGITAL_DOWN);

		// Receives Sensors
		leftInertialSensor = 0; // TODO
		rightInertialSensor = 0; // TODO

		// Sets Targets
		mutex.take(maxInterval); // Begin Exclusivity

		leftDriveTarget = leftStick;
		rightDriveTarget = rightStick;

		if (upButton == 1 && downButton == 0) {
			pistonTarget = true;
		}
		else if (upButton == 0 && downButton == 1) {
			pistonTarget = false;
		}

		mutex.give(); // End Exclusivity

		// Wait
		pros::delay(waitInterval);
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

		// Sets Electronics
		mutex.take(maxInterval); // Begin Exclusivity
		
		rightDriveMotors.move(PID(rightDriveTarget, rightInertialSensor, 1, 0, 0, &rightDriveI, 10, &rightDriveLastError));
		rightDriveMotors.move(PID(leftDriveTarget, leftInertialSensor, 1, 0, 0, &leftDriveI, 10, &leftDriveLastError));
		backPistons.set_value(pistonTarget);

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