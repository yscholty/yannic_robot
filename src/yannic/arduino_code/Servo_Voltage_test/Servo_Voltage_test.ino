// move the Servo to position 0 degree and note the analog value in the serial monitor.
// repeat for 180 degrees and note.
// -> adjust values in the arduino code describing the ros nodes on the UNO

//Version 1 #Yannic Scholtyssek

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.println(sensorValue);
}
