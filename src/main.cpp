#include <AccelStepper.h>
#define ENA 27         // pin connected to ENA+
#define home_switch 33 // pin 33 connected to Homeswitch
#define PUL 25         // pin connected to PUL+
#define DIR 26         // pin connected to DIR+

// Define a stepper and the pins it will use
AccelStepper stepperZ(1, PUL, DIR); // AccelStepper Setup [1]1=Stepper Mode, [2] PUL+ Pin, [3] DIR+ pin

// Stepper Travel Variables
long TravelZ;             // Used to store the Z value
int homing_finished = 1;  // used to check if homing is finished
int moved_down = 1;       // used to check if move is finished
int moved_up = 1;         // used to check if move is finished
long startup_homing = -1; // Used to Home the Motor at startup

// Stepper steps calculation
int Hub = 200;                                               // Used to store the desired Hub in mm
int ScrewPitch = 20;                                         // Used to store the screw pitch in mm
float BeltRatio = 56.0 / 15.0;                               // Used to store the belt transmission ratio
int MotorSteps = 800;                                        // Used to store the steps per revolution
int StepCount = (Hub / ScrewPitch) * BeltRatio * MotorSteps; // Steps of the Motor for the given Hub

void setup()
{

  Serial.begin(115200);

  stepperZ.setMaxSpeed(100);     // Set Max Speed of Motor for Homing
  stepperZ.setAcceleration(100); // Set Acceleration of Motor for Homing

  pinMode(ENA, OUTPUT);
  digitalWrite(ENA, LOW);
  homing_finished = 0;
  moved_down = 0;
  moved_up = 0;
}

void homing()
{

  pinMode(home_switch, INPUT_PULLUP);

  Serial.print("Stepper Motor is Homing........");

  while (digitalRead(home_switch))
  {                                  // Make the Motor move CCW until the switch is activated
    stepperZ.moveTo(startup_homing); // Set the position to move to
    startup_homing--;                // Decrease by 1 for next move if needed
    stepperZ.run();                  // Start moving the Motor
    delay(5);
  }

  stepperZ.setCurrentPosition(0);  // Set the current position as zero for now
  stepperZ.setMaxSpeed(100.0);     // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperZ.setAcceleration(100.0); // Set Acceleration of Stepper
  startup_homing = 1;

  while (!digitalRead(home_switch))
  { // Make the Stepper move CW until the switch is deactivated
    stepperZ.moveTo(startup_homing);
    stepperZ.run();
    startup_homing++;
    delay(5);
  }

  stepperZ.setCurrentPosition(0);
  Serial.println("Homing Completed");
  Serial.println("");
  homing_finished = 1;
  delay(2000);
}
void down()
{
  Serial.print("Moving Stepper down.....");
  while (moved_down == 0 && (moved_up == 1))
  {
    stepperZ.moveTo(StepCount);

    if ((stepperZ.distanceToGo() != 0))
    {
      stepperZ.run(); // Move Stepper into position
    }

    if (stepperZ.distanceToGo() == 0)
    {
      Serial.println("COMPLETED!");
      Serial.println("");
      moved_down = 1; // Reset move variable
      moved_up = 0;   // Reset move variable
      delay(2000);
    }
  }
}
void up()
{
  Serial.print("Moving Stepper up.....");

  while ((moved_up == 0) && (moved_down == 1))
  {
    stepperZ.moveTo(800);

    if ((stepperZ.distanceToGo() != 0))
    {
      stepperZ.run(); // Move Stepper into position
    }
    if (stepperZ.distanceToGo() == 0)
    {
      Serial.println("COMPLETED!");
      Serial.println("");
      moved_down = 0; // Reset move variable
      moved_up = 1;   // Reset move variable
      delay(5000);
    }
  }
}
void loop()
{

  if (homing_finished == 0) // check if homing was already executed
  {
    homing();
    moved_up = 1;
  }

  stepperZ.setMaxSpeed(2800.0);     // Set Max Speed of Stepper (Faster for regular movements)
  stepperZ.setAcceleration(2400.0); // Set Acceleration of Stepper

  if ((stepperZ.distanceToGo() == 0) && (moved_up == 1))
  {
    down();
  }

  if ((stepperZ.distanceToGo()) == 0 && (moved_down == 1))
  {
    up();
  }
}