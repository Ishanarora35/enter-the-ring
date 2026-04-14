#include <ESP32Servo.h>  // Includes the ESP32 servo library so the ESP32 can control hobby servos.

const int sensorPins[] = {34, 35, 36, 39};  // Stores the four EMG sensor input pins in an array.

const int sensorCount = sizeof(sensorPins) / sizeof(sensorPins[0]);  // Counts how many sensor pins are in the array.

const int servoPins[] = {18, 19, 21};  // Stores three example servo signal pins in an array.

const int servoCount = sizeof(servoPins) / sizeof(servoPins[0]);  // Counts how many servo pins are in the array.

Servo servo1;  // Creates the first servo object.
Servo servo2;  // Creates the second servo object.
Servo servo3;  // Creates the third servo object.

Servo* servos[] = {&servo1, &servo2, &servo3};  // Stores pointers to the three servo objects so we can loop through them.

const float alpha = 0.1f;  // Sets the smoothing amount for the EMG filter.

const int sampleDelayMs = 50;  // Sets the delay between each loop cycle in milliseconds.

const int sensorMin = 0;  // Says what sensor value should become 0 degrees on the servo.

const int sensorMax = 4095;  // Says what sensor value should become 180 degrees on the servo.

int rawValues[sensorCount] = {0};  // Creates an array to hold the newest raw sensor values.

float filteredValues[sensorCount] = {0.0f};  // Creates an array to hold the smoothed sensor values.

int servoAngles[servoCount] = {0};  // Creates an array to hold the angle for each servo.

void setup() {  // Runs one time when the board first turns on or resets.
  Serial.begin(115200);  // Starts serial communication at 115200 baud.

  delay(500);  // Waits half a second so the serial connection can settle.

  for (int i = 0; i < sensorCount; i++) {  // Starts a loop that goes through every sensor pin.
    pinMode(sensorPins[i], INPUT);  // Sets the current sensor pin to input mode.
    rawValues[i] = analogRead(sensorPins[i]);  // Reads the first raw value from the current sensor.
    filteredValues[i] = rawValues[i];  // Uses that first raw value as the starting filtered value.
  }  // Ends the sensor setup loop.

  ESP32PWM::allocateTimer(0);  // Reserves PWM timer 0 for ESP32 servo control.
  ESP32PWM::allocateTimer(1);  // Reserves PWM timer 1 for ESP32 servo control.
  ESP32PWM::allocateTimer(2);  // Reserves PWM timer 2 for ESP32 servo control.
  ESP32PWM::allocateTimer(3);  // Reserves PWM timer 3 for ESP32 servo control.

  servo1.setPeriodHertz(50);  // Sets servo 1 to the standard 50 Hz servo signal frequency.
  servo2.setPeriodHertz(50);  // Sets servo 2 to the standard 50 Hz servo signal frequency.
  servo3.setPeriodHertz(50);  // Sets servo 3 to the standard 50 Hz servo signal frequency.

  servo1.attach(servoPins[0], 500, 2400);  // Attaches servo 1 to its pin with a common pulse width range.
  servo2.attach(servoPins[1], 500, 2400);  // Attaches servo 2 to its pin with a common pulse width range.
  servo3.attach(servoPins[2], 500, 2400);  // Attaches servo 3 to its pin with a common pulse width range.

  for (int i = 0; i < servoCount; i++) {  // Starts a loop that goes through each servo.
    servoAngles[i] = 0;  // Sets the starting angle for the current servo to 0 degrees.
    servos[i]->write(servoAngles[i]);  // Sends the current servo to that starting angle.
  }  // Ends the servo startup loop.

  Serial.println("EMG + Servo control started");  // Prints a startup message to the Serial Monitor.
}  // Ends the setup function.

void loop() {  // Runs over and over forever after setup finishes.
  for (int i = 0; i < sensorCount; i++) {  // Starts a loop that reads and filters every sensor.
    rawValues[i] = analogRead(sensorPins[i]);  // Reads the newest raw value from the current sensor pin.

    filteredValues[i] = (alpha * rawValues[i]) + ((1.0f - alpha) * filteredValues[i]);  // Blends the new sensor value with the old filtered value.
  }  // Ends the sensor reading loop.

  for (int i = 0; i < servoCount; i++) {  // Starts a loop that updates each servo.
    servoAngles[i] = map((int)filteredValues[i], sensorMin, sensorMax, 0, 180);  // Converts the filtered sensor value into a servo angle.

    servoAngles[i] = constrain(servoAngles[i], 0, 180);  // Keeps the servo angle between 0 and 180 degrees.

    servos[i]->write(servoAngles[i]);  // Moves the current servo to the calculated angle.
  }  // Ends the servo update loop.

  for (int i = 0; i < sensorCount; i++) {  // Starts a loop that prints each sensor value.
    Serial.print("Sensor");  // Prints the word Sensor.
    Serial.print(i + 1);  // Prints the sensor number, starting at 1 instead of 0.
    Serial.print(":");  // Prints a colon after the sensor number.
    Serial.print(filteredValues[i], 2);  // Prints the filtered sensor value with 2 decimal places.

    if (i < sensorCount - 1) {  // Checks whether this is not the last sensor in the list.
      Serial.print(", ");  // Prints a comma and space between sensor values.
    }  // Ends the separator check.
  }  // Ends the sensor print loop.

  Serial.print(", Servo1:");  // Prints a label for servo 1.
  Serial.print(servoAngles[0]);  // Prints the current angle for servo 1.
  Serial.print(", Servo2:");  // Prints a label for servo 2.
  Serial.print(servoAngles[1]);  // Prints the current angle for servo 2.
  Serial.print(", Servo3:");  // Prints a label for servo 3.
  Serial.print(servoAngles[2]);  // Prints the current angle for servo 3.

  Serial.println();  // Ends the line so the next loop prints on a new line.

  delay(sampleDelayMs);  // Waits a short time before doing the next full loop.
}  // Ends the loop function.
