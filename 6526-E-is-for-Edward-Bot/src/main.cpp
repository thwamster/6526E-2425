#include "main.h"
using namespace pros;

/* Global Objects */
// Constant Intervals
const int waitInterval = 12; //wait 12 ms
const int maxInterval = 100;


// Tasks
Mutex mutex;

// Electronics
/* 
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
 * Fourbar
 * 	  left: port 16
 * 	  right port 17
 * 
 * Inertia sensors:
 * 	  left side: port X
 * 	  right side: port X
 * 
 * Radio: port 21
 * 
 * Clamp Pistons: 3 wire output A
 * 
 * Fourbar encoders: 
 * 
 */

Controller master(E_CONTROLLER_MASTER);

//drive
MotorGroup leftDriveMotors({-9, -10});
MotorGroup rightDriveMotors({1,2});

//intake and score mech
Motor intake(7);
Motor score(8);

//fourbar
Motor fourbarMotorR (16);
Motor fourbarMotorL (-17);

//clamp
adi::Pneumatics clampPistons (1, true);

// Target & Sensor Variables
//*o7 note: so these are global vars that are given values in the getInputs func and then 
//used to intiate function in the controls func. Technically, we can just use the raw inputs instead and make the vars
//that said inputs are stored in global.
// Initialize Variables
int leftDriveTarget{};
int rightDriveTarget{};

bool enablePID{false};
int leftInertialSensor{};
int rightInertialSensor{};

bool pistonTarget{false};
int intakeScoreTarget{};
int fourbarTarget {};

/* Competition Methods */
/* 
 * TODO
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
 * TODO
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
void autonomous() {

	leftDriveMotors.move(127);
	rightDriveMotors.move(127);

	delay(2000);

	leftDriveMotors.move(-127);
	rightDriveMotors.move(127);

	delay (500);
}

/* 
 * TODO
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
 * TODO
 */
void getInputs() {
	
	//create and intitialize vars to store controller input data and enable functions 
	const int doubleInputDelay = 200;

	//tank drive joysticks
	int leftStick {};
	int rightStick {};

	//Drive PID buttons and delay 
	int aButton {};
	int aDelayCount {};

	//clamp piston buttons
	int upButton {};
	int downButton {};

	//intake-score buttons
	int rBumper1 {};
	int rBumper2 {};

	//fourbar buttons
	int lBumper1 {};
	int lBumper2 {};

	// Input and Electronics Loop
	while (true) {
		lcd::set_text(2, "Electronics updating.");

		// Receives Inputs

		//drive joystick inputs
		leftStick = master.get_analog(ANALOG_LEFT_Y);
		rightStick = master.get_analog(ANALOG_RIGHT_Y);

		//clamp pistons buttons inputs
		upButton = master.get_digital(DIGITAL_UP);
		downButton = master.get_digital(DIGITAL_DOWN);

		//intake-score button imputs
		rBumper1 = master.get_digital(DIGITAL_R1);
		rBumper2 = master.get_digital(DIGITAL_R2);

		//fourbar inputs
		lBumper1 = master.get_digital(DIGITAL_L1);
		lBumper2 = master.get_digital(DIGITAL_L2);

		//enable/disable PID
		aButton = master.get_digital(DIGITAL_A);

		// Receives Sensors
		leftInertialSensor = 0; // TODO
		rightInertialSensor = 0; // TODO


		// Set Targets

		mutex.take(maxInterval); // Begin Exclusivity

		//drive input to target
		leftDriveTarget = leftStick;
		rightDriveTarget = rightStick;

		//Enable drive PID input
		if (aButton == 1 && aDelayCount >= doubleInputDelay && false) {
			enablePID = !enablePID;
			aDelayCount = 0;
			lcd::set_text(5, "PID enabled.");
		}

		//piston on/off
		if (upButton == 1 && downButton == 0) {
			pistonTarget = true;
			lcd::set_text(4, "Pneumatics enabled.");
		}
		else if (upButton == 0 && downButton == 1){
			pistonTarget = false;
			lcd::set_text(4, "Pneumatics disabled.");
		}

		//intake-score on/off
		if (rBumper1==1 && rBumper2 == 0)
		{
			intakeScoreTarget = 1;
			lcd::set_text(6, "intake forward (intake).");
		}else if (rBumper1==0 && rBumper2==1)
		{
			intakeScoreTarget = 2;
			lcd::set_text(6,"intake reverse (output)");
		}else
		{
			intakeScoreTarget=0;
			lcd::set_text(6, "intake off");
		}

		//fourbar on/off
		if (lBumper1==1 && lBumper2 == 0)
		{
			fourbarTarget = 1;
			lcd::set_text(7, "fourbar up (forward).");
		}else if (lBumper1==0 && lBumper2==1)
		{
			fourbarTarget = 2;
			lcd::set_text(7,"fourbar down (reverse)");
		}else
		{
			fourbarTarget=0;
			lcd::set_text(7, "fourbar off");
		}

		mutex.give(); // End Exclusivity

		// Wait
		delay(waitInterval);
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
	lcd::set_text(3, "Control system processing.");
	while (true) {

		// Sets Electronics
		mutex.take(maxInterval); // Begin Exclusivity

		//Drive Movement
		if (enablePID) {		//Drive PID alteration
			rightDriveMotors.move(PID(rightDriveTarget, rightInertialSensor, 1, 0, 0, &rightDriveI, 10, &rightDriveLastError));
			leftDriveMotors.move(PID(leftDriveTarget, leftInertialSensor, 1, 0, 0, &leftDriveI, 10, &leftDriveLastError));
		}
		else {
			rightDriveMotors.move(rightDriveTarget);
			leftDriveMotors.move(leftDriveTarget);
			lcd::set_text(5, "PID disabled.");
		}

		//clamp piston movememnt
		if (pistonTarget){
			clampPistons.extend();
		}
		else{
			clampPistons.retract();
		}

		//intake-score movement
		if (intakeScoreTarget == 1){
			intake.move(90);
			score.move(80);
		}else if (intakeScoreTarget == 2)
		{
			intake.move(-90);
			score.move(-80);
			
		} else if (intakeScoreTarget == 0)
		{
			intake.move(0);
			score.move(0);
		}

		//fourbar movement					//o7 need to add motors for the fourbar
		if (fourbarTarget == 2)
		{
			fourbarMotorR.move(127);
			fourbarMotorL.move(127);
		}else if (fourbarTarget == 1)
		{
			fourbarMotorR.move(-127);
			fourbarMotorL.move(-127);
		} else if (fourbarTarget == 0)
		{
			fourbarMotorR.move(0);
			fourbarMotorL.move(0);

		}


		mutex.give(); // End Exclusivity

		delay(waitInterval);   //wait
	}
	lcd::set_text(3, "Error in control system loop");
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
