#include "main.h"
using namespace pros;

/* Global Objects */
// Constant Intervals
const int waitInterval = 12;
const int maxInterval = 100;
const int doubleInputDelay = 200;

// Tasks
Mutex mutex;

/* 
 * Electronics
 *
 * Drivetrain: 
 * 	  left front: port 10
 * 	  left back: port 9
 * 	  right front: port 2
 * 	  right back: port 1
 * 
 * Intake-score:
 * 	  intake: port 7 
 * 	  score: port 8
 * 
 * Fourbar:
 * 	  left: port 16
 * 	  right port 17
 * 
 * Inertial Sensors:
 * 	  left side: port 2
 * 	  right side: port 1
 * 
 * Radio: port 21
 * 
 * Clamp Pistons: 3 wire output A
 * 
 * Fourbar encoders: 
 * 
 */
// Controller
Controller master(E_CONTROLLER_MASTER);

// Drive Motors
MotorGroup leftDriveMotors({-9, -10});
MotorGroup rightDriveMotors({1,2});

// Intake and Score Mechanism
Motor intakeMotor(7);
Motor scoreMotor(8);

// Fourbar Mechanism
Motor rightFourbarMotor (16);
Motor leftFourbarMotor (-17);

// Clamp Mechanism
adi::Pneumatics clampPistons (1, true);

// Sensors
pros::Imu leftInertialSensor(19);
pros::Imu rightInertialSensor(20);

/* Target & Sensor Variables */

// *o7 note: so these are global vars that are given values in the getInputs func and then 
// used to intiate function in the controls func. Technically, we can just use the raw inputs instead and make the vars
// that said inputs are stored in global.

// Initialize Variables
int leftDriveTarget{};
int rightDriveTarget{};
bool enablePID{false};
bool pistonTarget{false};
int intakeScoreTarget{};
int fourbarTarget{};
int leftDriveActual;
int rightDriveActual;

/* Competition Methods */
/* 
 * Center Button reaction method.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(2, "I was pressed!");
	} else {
		lcd::clear_line(2);
	}
}

/* 
 * Initialization Method.
 */
void initialize() {
	lcd::initialize();
	lcd::set_text(1, "Initialized.");
		
	//	mutex.take(maxInterval); // Begin Exclusivity !o7: you could probs remove mutex if the var initialization didn't matter
	//
	//	// original Initialize Variables !o7: double check that you didn't fuck it up by moving the initialization
	//
	//	// Initialize Tasks
	//	mutex.give(); // End Exclusivity

	lcd::register_btn1_cb(on_center_button);
}

/* 
 * Disabled state. Unused.
 */
void disabled() {}

/* 
 * Competition initialization method. Unused.
 */
void competition_initialize() {}

/* 
 * Autonomous control method.
 */
void autonomous() {
	leftDriveMotors.move(127);
	rightDriveMotors.move(127);
	delay(2000);

	leftDriveMotors.move(-127);
	rightDriveMotors.move(127);
	delay (500);
}

/* 
 * Operator control method.
 */
void opcontrol() {
	Task inputs(getInputs);
	Task drive(driveControlSystem);

	while (true) {
		lcd::set_text(1, "Under operator control.");

		// Wait
		delay(waitInterval);
	}
}

/* Control Methods */
/* 
 * Receives inputs from controllers and sensors and updates global variables.
 */
void getInputs() {
	// Tank Drive Joysticks
	int leftStick{};
	int rightStick{};

	// Drive PID buttons and delay for said button 
	int aButton{};
	int aDelayCount{};

	// Clamp piston buttons
	int upButton{};
	int downButton{};

	// Intake-score buttons
	int r1Bumper{};
	int r2Bumper{};

	// Fourbar buttons
	int l1Bumper{};
	int l2Bumper{};

	// Sensors
	double leftVelocity{};
	double rightVelocity{};

	// Input and Electronics Loop
	while (true) {
		lcd::set_text(2, "Electronics updating.");

		// Receives Inputs

		// Drive joystick inputs
		leftStick = master.get_analog(ANALOG_LEFT_Y);
		rightStick = master.get_analog(ANALOG_RIGHT_Y);

		// Clamp pistons buttons inputs
		upButton = master.get_digital(DIGITAL_UP);
		downButton = master.get_digital(DIGITAL_DOWN);

		// Intake-score button imputs
		r1Bumper = master.get_digital(DIGITAL_R1);
		r2Bumper = master.get_digital(DIGITAL_R2);

		// Fourbar inputs
		l1Bumper = master.get_digital(DIGITAL_L1);
		l2Bumper = master.get_digital(DIGITAL_L2);

		// Enable/disable PID
		aButton = master.get_digital(DIGITAL_A);

		// Receives Sensors
		leftVelocity = leftInertialSensor.get_accel().y;
		rightVelocity = -1 * rightInertialSensor.get_accel().y;

		// Sets Targets
		mutex.take(maxInterval); // Begin Exclusivity

		// Drive input to target
		leftDriveTarget = leftStick;
		rightDriveTarget = rightStick;

		// Enable drive PID input. Unused.
		if (aButton == 1 && aDelayCount >= doubleInputDelay && false) {
			enablePID = !enablePID;
			aDelayCount = 0;
			
			if (enablePID) {
				lcd::set_text(5, "PID enabled.");
			}
			else {
				lcd::set_text(5, "PID disabled.");
			}
		}

		// Piston on/off
		if (upButton == 1 && downButton == 0) {
			pistonTarget = true;
			lcd::set_text(4, "Pneumatics enabled.");
		}
		else if (upButton == 0 && downButton == 1){
			pistonTarget = false;
			lcd::set_text(4, "Pneumatics disabled.");
		}

		// Intake-score on/off
		if (r1Bumper == 1 && r2Bumper == 0) {
			intakeScoreTarget = 1;
			lcd::set_text(6, "Intake forward (intake).");
		}
		else if (r1Bumper == 0 && r2Bumper == 1) {
			intakeScoreTarget = 2;
			lcd::set_text(6,"Intake reverse (output)");
		}
		else {
			intakeScoreTarget = 0;
			lcd::set_text(6, "Intake off");
		}

		//fourbar on/off
		if (l1Bumper == 1 && l2Bumper == 0) {
			fourbarTarget = 1;
			lcd::set_text(7, "Fourbar up (forward).");
		}
		else if (l1Bumper == 0 && l2Bumper == 1) {
			fourbarTarget = 2;
			lcd::set_text(7,"Fourbar down (reverse).");
		}
		else {
			fourbarTarget = 0;
			lcd::set_text(7, "Fourbar off.");
		}

		mutex.give(); // End Exclusivity

		// Wait
		delay(waitInterval);
		aDelayCount++;
	}
}

/* 
 * Driver control system. Updates motors and electronics from target variables.
 */
void driveControlSystem() {
	int rightDriveI = 0;
	int rightDriveLastError = 0;

	int leftDriveI = 0;
	int leftDriveLastError = 0;

	// Drive Control System Loop
	lcd::set_text(3, "Control system processing.");
	while (true) {

		// Sets Electronics
		mutex.take(maxInterval); // Begin Exclusivity

		// Drive Movement
		if (enablePID) {
			rightDriveMotors.move(PID(rightDriveTarget, rightDriveActual, 1, 0, 0, &rightDriveI, 10, &rightDriveLastError));
			leftDriveMotors.move(PID(leftDriveTarget, leftDriveActual, 1, 0, 0, &leftDriveI, 10, &leftDriveLastError));
		}
		else {
			rightDriveMotors.move(rightDriveTarget);
			leftDriveMotors.move(leftDriveTarget);
			lcd::set_text(5, "PID disabled.");
		}

		// Clamp piston movememnt
		if (pistonTarget){
			clampPistons.extend();
		}
		else{
			clampPistons.retract();
		}

		// Intake-score movement
		if (intakeScoreTarget == 1){
			intakeMotor.move(90);
			scoreMotor.move(80);
		}else if (intakeScoreTarget == 2)
		{
			intakeMotor.move(-90);
			scoreMotor.move(-80);
			
		} else if (intakeScoreTarget == 0)
		{
			intakeMotor.move(0);
			scoreMotor.move(0);
		}

		// Fourbar movement
		if (fourbarTarget == 2) {
			rightFourbarMotor.move(127);
			leftFourbarMotor.move(127);
		}
		else if (fourbarTarget == 1) {
			rightFourbarMotor.move(-127);
			leftFourbarMotor.move(-127);
		}
		else if (fourbarTarget == 0) {
			rightFourbarMotor.move(0);
			leftFourbarMotor.move(0);
		}

		mutex.give(); // End Exclusivity

		delay(waitInterval);
	}
	lcd::set_text(3, "Error in control system loop.");
}


/* 
 * PID control system method.
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