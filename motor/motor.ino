#include <Wire.h>
//Motor A
const int motorPin1  = 2;
const int motorPin2  = 3;  

//Motor B
const int motorPin3  = 4; 
const int motorPin4  = 5;  

//Power Configuration
const int lowPower = 40;
const int midPower = 50;
const int highPower = 150;

void setup() {
  //Set pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  Wire.begin(8);      // Start I2C communication with address 8
  Wire.onReceive(receiveData); // Set up a callback function to receive data
  Serial.begin(9600); // Start serial for output
  
}

void moveForward(int power){
  // Serial.println("Move forward");
  analogWrite(motorPin1, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin2, power+1);
  analogWrite(motorPin4, power);
}

void moveBackward(int power, int timeDelay){
  Serial.println("Move Backward");
  analogWrite(motorPin2, 0);
  analogWrite(motorPin4, 0);
  analogWrite(motorPin1, power);
  analogWrite(motorPin3, power);
  delay(timeDelay);
}

void turnRight(int power, int turnDelay){
  Serial.println("Turn Right");
  analogWrite(motorPin1, power);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, power);
  delay(turnDelay);
}

void turnLeft(int power, int turnDelay){
  Serial.println("Turn Left");
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, power);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 0);
  delay(turnDelay);
}

void stop(int stopDelay){
  Serial.println("Stop");
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 0);
  delay(stopDelay);
}


void loop() {
}

void receiveData(int byteCount) {
  if (byteCount <= 32) { // Assuming the maximum string length is 32 characters, adjust as needed
    char receivedCharString[33]; // Allocate space for the received string

    for (int i = 0; i < byteCount; i++) {
      receivedCharString[i] = Wire.read(); // Read each character into the string
    }
    receivedCharString[byteCount] = '\0'; // Null-terminate the string
    String receivedString = String(receivedCharString);
    Serial.println(receivedString);
    
    //Use a switch statement to check the received string
    if (strcmp(receivedCharString, "move_forward") == 0) {
      moveForward(midPower);
    } else if (strcmp(receivedCharString, "turn_left") == 0) {
      turnLeft(midPower, 0.55);
    } else if (strcmp(receivedCharString, "turn_right") == 0) {
      turnRight(midPower, 0.55);
    } else if (strcmp(receivedCharString, "move_backward") == 0) {
      moveBackward(midPower, 0);
    } else if (strcmp(receivedCharString, "stop") == 0) {
      stop(10);
    }
  }
}


