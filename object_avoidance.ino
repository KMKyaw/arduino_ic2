#include <Wire.h>

// Front ultrasonic
const int trigPin1 = 2;
const int echoPin1 = 3;

// Front-right ultrasonic
const int trigPin2 = 4;
const int echoPin2 = 5;

// Front-left ultrasonic
const int trigPin3 = 6;
const int echoPin3 = 7;

// Right ultrasonic
const int trigPin4 = 8;
const int echoPin4 = 9;

// Left ultrasonic
const int trigPin5 = 10;
const int echoPin5 = 11;

// Back ultrasonic
const int trigPin6 = 12;
const int echoPin6 = 13; 

void setup() {
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT); 

  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 

  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT); 

  pinMode(trigPin4, OUTPUT); 
  pinMode(echoPin4, INPUT); 

  pinMode(trigPin5, OUTPUT); 
  pinMode(echoPin5, INPUT); 

  pinMode(trigPin6, OUTPUT); 
  pinMode(echoPin6, INPUT); 
   
  Wire.begin(); 
  Serial.begin(9600);
}

int getFrontDist(){
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  int duration = pulseIn(echoPin1, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

int getFrontRightDist(){
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  int duration = pulseIn(echoPin2, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

int getFrontLeftDist(){
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  int duration = pulseIn(echoPin3, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

int getRightDist(){
  digitalWrite(trigPin4, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin4, LOW);
  int duration = pulseIn(echoPin4, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

int getLeftDist(){
  digitalWrite(trigPin5, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin5, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin5, LOW);
  int duration = pulseIn(echoPin5, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

int getBackDist(){
  digitalWrite(trigPin6, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin6, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin6, LOW);
  int duration = pulseIn(echoPin6, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void sendMessage(String message) {
  Serial.println(message);
  Wire.beginTransmission(8);
  char msgChar[50];
  message.toCharArray(msgChar, 50);  
  Wire.write(msgChar, strlen(msgChar));  
  Wire.endTransmission();
}

void run(int turningDelay, int safeDist){
  int frontDist = getFrontDist();

  // If there's an obstacle in the front
  if (frontDist < safeDist){  
    int rightDist = getRightDist();
    int leftDist = getLeftDist();

    // Right and left are blocked
    if (rightDist < safeDist && leftDist < safeDist) {
      int backDist = getBackDist();
      while (leftDist < safeDist && rightDist < safeDist && backDist > safeDist){
        sendMessage("move_backward");
        
        rightDist = getRightDist();
        leftDist = getLeftDist();
      }
      if (leftDist > safeDist) {
        sendMessage("turn_left");
      }
      else {
        sendMessage("turn_right");
      }
    }

    // Right and left are not blocked
    else if (rightDist > safeDist && leftDist > safeDist){
      int frontLeftDist = getFrontLeftDist();
      while (frontDist < safeDist || frontLeftDist < 30){
        sendMessage("turn_right");
        frontDist = getFrontDist();
        frontLeftDist = getFrontLeftDist();
      }
    }
 
    // Left side is blocked
    else if (leftDist < safeDist){
      int frontLeftDist = getFrontLeftDist();
      while (frontDist < safeDist || frontLeftDist < 30){
        sendMessage("turn_right");
        frontDist = getFrontDist();
        frontLeftDist = getFrontLeftDist();
      }
    }

    // Right side is blocked
    else if (rightDist < safeDist){
      int frontRightDist = getFrontRightDist();
      while (frontDist < safeDist || frontRightDist < 30){
        sendMessage("turn_left");
        frontDist = getFrontDist();
        frontRightDist = getFrontRightDist();
      }
    }
  }
  else{
    int rightDist = getRightDist();
    int leftDist = getLeftDist();
    int frontRightDist = getFrontRightDist();
    int frontLeftDist = getFrontLeftDist();

    if (frontRightDist < 30 || rightDist < 20){
      sendMessage("turn_left");
    } 
    else if (frontLeftDist < 30 || leftDist < 20){
      sendMessage("turn_right");
    }
    else {
      sendMessage("move_forward");
    }
  }
}

void loop() {
  // int turningDelay = (9 * 80 / power) * 100;
  int turningDelay = 0;
  int safeDist = 60;
  run(turningDelay, safeDist);
}