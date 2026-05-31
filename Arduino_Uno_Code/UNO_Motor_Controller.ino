#include <AFMotor.h>
#include <SoftwareSerial.h>

// ESP32 serial on A0 = RX, A1 = TX
SoftwareSerial esp(14, 15);   // RX, TX

AF_DCMotor leftMotor(1);   // M1
AF_DCMotor rightMotor(2);  // M2

int currentSpeed = 180;

// Change these if one side moves opposite
bool LEFT_INVERT = false;
bool RIGHT_INVERT = false;

char cmdBuffer[20];
byte cmdIndex = 0;

void setMotor(AF_DCMotor &motor, bool forward, bool invert) {
  bool dir = forward ^ invert;
  motor.setSpeed(currentSpeed);
  motor.run(dir ? FORWARD : BACKWARD);
}

void stopMotors() {
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}

void forwardMove() {
  setMotor(leftMotor, true, LEFT_INVERT);
  setMotor(rightMotor, true, RIGHT_INVERT);
}

void backwardMove() {
  setMotor(leftMotor, false, LEFT_INVERT);
  setMotor(rightMotor, false, RIGHT_INVERT);
}

void leftMove() {
  setMotor(leftMotor, false, LEFT_INVERT);
  setMotor(rightMotor, true, RIGHT_INVERT);
}

void rightMove() {
  setMotor(leftMotor, true, LEFT_INVERT);
  setMotor(rightMotor, false, RIGHT_INVERT);
}

void handleCommand(char *cmd) {
  if (cmd[0] == 'F') {
    forwardMove();
  } 
  else if (cmd[0] == 'B') {
    backwardMove();
  } 
  else if (cmd[0] == 'L') {
    leftMove();
  } 
  else if (cmd[0] == 'R') {
    rightMove();
  } 
  else if (cmd[0] == 'S') {
    stopMotors();
  } 
  else if (cmd[0] == 'V') {
    int spd = atoi(cmd + 1);
    currentSpeed = constrain(spd, 0, 255);
  }
}

void setup() {
  Serial.begin(115200);
  esp.begin(115200);

  leftMotor.setSpeed(currentSpeed);
  rightMotor.setSpeed(currentSpeed);

  stopMotors();

  Serial.println("UNO READY");
}

void loop() {
  while (esp.available()) {
    char c = esp.read();

    if (c == '\n' || c == '\r') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = '\0';
        handleCommand(cmdBuffer);
        cmdIndex = 0;
      }
    } else {
      if (cmdIndex < sizeof(cmdBuffer) - 1) {
        cmdBuffer[cmdIndex++] = c;
      }
    }
  }
}
