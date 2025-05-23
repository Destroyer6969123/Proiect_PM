#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

// Pin buzzer pe OC0A (D6 / PD6)
#define BUZZER_PIN PD6  // D6 = OC0A

// Pini motor (DRV8833)
#define IN1 PD2 // D2
#define IN2 PD3 // D3
#define IN3 PD4 // D4
#define IN4 PD5 // D5

// Bluetooth
SoftwareSerial bluetooth(8, 7); // TX, RX

// Servomotoare
Servo turretTilt;     // pin 12
Servo turretRotate;   // pin 13
int angleTilt = 90;
int angleRotate = 90;
unsigned long lastServoCmd = 0;
unsigned long servoTimeout = 200;
char currentServoCmd = 0;

// PWM motor
uint8_t speedPWM = 127;
volatile bool braked = false;
char lastChar = 0;
unsigned long lastMotorCmd = 0;
unsigned long motorTimeout = 200;
bool motorsActive = false;

void setupPWM() {
  // PWM pe D9 și D10 (Timer1)
  DDRB |= (1 << PB1) | (1 << PB2);
  TCCR1A = (1 << WGM10);
  TCCR1B = (1 << WGM12);
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
  TCCR1B |= (1 << CS11) | (1 << CS10);
  OCR1A = 0;
  OCR1B = 0;
}

void beepBuzzer(uint8_t durationMs) {
  DDRD |= (1 << BUZZER_PIN);
  TCCR0A |= (1 << COM0A0) | (1 << WGM01) | (1 << WGM00);
  TCCR0B |= (1 << CS01);
  OCR0A = 128;
  delay(durationMs);
  TCCR0A &= ~(1 << COM0A0);
}

void stopMotors() {
  PORTD &= ~((1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4));
  OCR1A = 0;
  OCR1B = 0;
}

void brakeMotors() {
  PORTD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4);
  OCR1A = 0;
  OCR1B = 0;
  braked = true;
  beepBuzzer(100); // Buzzer on brake
}

template<typename T>
void swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

void moveDiagonal(char a, char b) {
  if (a > b) swap(a, b);
  stopMotors();

  if (a == 'A' && b == 'S') {
    PORTD &= ~(1 << IN3);
    OCR1B = speedPWM;
  } else if (a == 'A' && b == 'W') {
    PORTD |= (1 << IN3);
    OCR1B = speedPWM;
  } else if (a == 'D' && b == 'S') {
    PORTD &= ~(1 << IN1);
    OCR1A = speedPWM;
  } else if (a == 'D' && b == 'W') {
    PORTD |= (1 << IN1);
    OCR1A = speedPWM;
  }
}

void handleCommand(char cmd) {
  Serial.print("Comandă primită: ");
  Serial.println(cmd);
  char upper = toupper(cmd);
  stopMotors();

  if (upper == 'E') {
    if (speedPWM < 255) speedPWM = min(speedPWM + 25, 255);
    if (speedPWM == 255) beepBuzzer(100);
    return;
  }

  if (upper == 'Q') {
    if (speedPWM > 0) speedPWM = max(speedPWM - 25, 0);
    return;
  }

  if (upper == 'X') {
    brakeMotors();
    return;
  }

  if (upper == 'W') {
    PORTD |= (1 << IN1) | (1 << IN3);
    OCR1A = speedPWM;
    OCR1B = speedPWM;
    lastMotorCmd = millis();
    motorsActive = true;
  } else if (upper == 'S') {
    PORTD |= (1 << IN2) | (1 << IN4);
    OCR1A = speedPWM;
    OCR1B = speedPWM;
    lastMotorCmd = millis();
    motorsActive = true;
  } else if (upper == 'A') {
    PORTD |= (1 << IN1) | (1 << IN3);
    PORTD &= ~(1 << IN1);
    PORTD |= (1 << IN3);
    OCR1A = speedPWM;
    OCR1B = speedPWM;
    lastMotorCmd = millis();
    motorsActive = true;
  } else if (upper == 'D') {
    PORTD |= (1 << IN1);
    PORTD &= ~(1 << IN3);
    OCR1A = speedPWM;
    OCR1B = speedPWM;
    lastMotorCmd = millis();
    motorsActive = true;
  }

  if ((lastChar == 'W' && (upper == 'A' || upper == 'D')) ||
      (lastChar == 'S' && (upper == 'A' || upper == 'D'))) {
    moveDiagonal(lastChar, upper);
    lastMotorCmd = millis();
    motorsActive = true;
  } else {
    lastChar = upper;
  }

  switch (upper) {
    case 'I': case 'K': case 'J': case 'L':
      currentServoCmd = upper;
      lastServoCmd = millis();
      break;
  }
}

void setup() {
  DDRD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4);
  setupPWM();

  turretTilt.attach(12);
  turretRotate.attach(13);
  turretTilt.write(angleTilt);
  turretRotate.write(angleRotate);

  Serial.begin(9600);
  bluetooth.begin(9600);
  stopMotors();
}

void loop() {
  if (bluetooth.available()) handleCommand(bluetooth.read());
  if (Serial.available()) handleCommand(Serial.read());

  if (motorsActive && (millis() - lastMotorCmd > motorTimeout)) {
    stopMotors();
    motorsActive = false;
  }

  if (millis() - lastServoCmd < servoTimeout) {
    switch (currentServoCmd) {
      case 'I': angleTilt = min(angleTilt + 1, 180); turretTilt.write(angleTilt); break;
      case 'K': angleTilt = max(angleTilt - 1, 0); turretTilt.write(angleTilt); break;
      case 'J': angleRotate = min(angleRotate + 1, 180); turretRotate.write(angleRotate); break;
      case 'L': angleRotate = max(angleRotate - 1, 0); turretRotate.write(angleRotate); break;
    }
  }
}
