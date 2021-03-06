/*
    Arduino side that Python sends commands to
  This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
  It won't work with v1.x motor shields! Only for the v2's with built in PWM
  control
  For use with the Adafruit Motor Shield v2
  ---->  http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *left_motor = AFMS.getMotor(2);
// You can also make another motor on port M2
Adafruit_DCMotor *right_motor = AFMS.getMotor(1);

const int CommandTimeout = 1000; // if no new command in this amount of time, go to 'N'

int current_left = 0;
int current_right = 0;
int target_left = 0;
int target_right = 0;

int front_left_sensor = A0;
int back_left_sensor = A1;
int front_right_sensor = A3;
int back_right_sensor = A2;
int front_left_obstacle;
int front_right_obstacle;
int back_left_obstacle;
int back_right_obstacle;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  pinMode( front_left_sensor, INPUT );
  pinMode( back_left_sensor, INPUT );
  pinMode( front_right_sensor, INPUT );
  pinMode( back_right_sensor, INPUT );

  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz


}

void loop() {
  stop_at_edge();
  respond_to_command();

}

void stop_left() {
  Serial.println("Stopleft");
  left_motor->setSpeed(0);
  current_left = 0;
  target_left = 0;
}

void stop_right() {
  Serial.println("Stoprigth");
  right_motor->setSpeed(0);
  current_right = 0;
  target_right = 0;

}

void stop_at_edge() {
  // for each IR sensor
  front_left_obstacle = ! digitalRead( front_left_sensor );
  front_right_obstacle = ! digitalRead( front_right_sensor );
  back_left_obstacle = ! digitalRead( back_left_sensor );
  back_right_obstacle = ! digitalRead( back_right_sensor );

  // if forward, stop on any front obstacle
  if ( (front_right_obstacle || front_left_obstacle) && ( target_right > 0 && target_left > 0) ) {
    stop_left();
    stop_right();
  }

  // if turning right, stop on right obstacle
  if ( front_right_obstacle && (target_left > 0 && target_right == 0 ) ) {
    stop_left();
  }

  // if turning left, stop on left obstacle
  if ( front_left_obstacle && (target_right > 0 && target_left == 0 )) {
    stop_right();
  }

  // going back, stop on any
  if ( ( back_left_obstacle || back_right_obstacle ) && ( current_right < 0 || current_left < 0 ) ) {
    stop_left();
    stop_right();
  }

}

void respond_to_command() {
  static unsigned long command_expire = 0; // start expired (before you do anything) // like a global

  boolean is_move_command = false;
  int command = 0;

  if (command_expire != 0 && millis() - command_expire > CommandTimeout) {
    command = 'S';
    Serial.print("Expired ");Serial.println(millis());
    command_expire = 0; // don't expire until command_expire is set to something
  }

  if (Serial.available() > 0 ) {
    // then we will handle it
    command = Serial.read();
  }

  // if there is a command
  if (command != 0) {
    switch (command) {
      case 'L' :
        if ( ! front_left_obstacle ) {
          target_left = 0;
          target_right = 150;
          is_move_command = true;
        }
        break;
      case 'R':
        if ( ! front_right_obstacle ) {
          target_left = 150;
          target_right = 0;
          is_move_command = true;
        }
        break;
      case 'F' :
        if ( ! (front_left_obstacle || front_right_obstacle) ) {
          target_left = 150;
          target_right = 150;
          is_move_command = true;
        }
        break;
      case 'B' :
        if ( ! (back_left_obstacle || back_right_obstacle) ) {
          target_left = -150;
          target_right = -150;
          is_move_command = true;
        }
        break;
      case 'N' :
      case 'S' :
        target_left = 0;
        target_right = 0;
        break;
      default :
        ;
    }

    if (is_move_command) {
      // reset expiration at each command
      command_expire = millis();
      // Serial.print("Expire at ");Serial.println(command_expire);
    }

    if (command != 'S') {
      Serial.print("Command "); Serial.print((char)command);
      Serial.print(" "); Serial.print(current_left); Serial.print("|"); Serial.print(target_left);
      Serial.print("/"); Serial.print(current_right); Serial.print("|"); Serial.print(target_right);
      Serial.println();
    }

  }

  // keep changing till we get to the target l/r
  // remeMber current
  //change towards new target
  if (current_left < target_left) {
    // Serial.print("+L");
    current_left = current_left + 1;
  }
  else if (current_left > target_left) {
    // Serial.print("-L");
    current_left = current_left - 1;
  }
  //if it is equal it will keep doing, dont need to code
  //if speed is -100 to -1 go backwards 0 is still 1 to 100 is forward
  if (current_left <= 0) {
    left_motor->run(BACKWARD);
  }
  else if (current_left >= 0) {
    left_motor->run(FORWARD);
  }
  left_motor->setSpeed(abs(current_left));

  if (current_right < target_right) {
    // Serial.print("+R");
    current_right = current_right + 1;
  }
  else if (current_right > target_right) {
    // Serial.print("-R");
    current_right = current_right - 1;
  }
  //if it is equal it will keep doing, dont need to code

  if (current_right <= 0) {
    right_motor->run(BACKWARD);
  }
  else if (current_right >= 0) {
    right_motor->run(FORWARD);
  }

  right_motor->setSpeed(abs(current_right));

  if (current_left != target_left || current_right != target_right) {
    // Serial.print(current_left); Serial.print("/"); Serial.print(current_right); Serial.println();
  }


}
