const int trigPin = 9;
const int trigPin2 = 5;
const int echoPin = 10;
const int echoPin2 = 6;
long duration;
int distance;
int parkingOneFree;
long duration2;
int distance2;
int parkingTwoFree;
void setup() {
  pinMode(trigPin, OUTPUT);   
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  Serial.begin(9600); 
}
void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delay(1000);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance = duration * 0.034 / 2;
  distance2 = duration2 * 0.034 / 2;
  if (distance > 50) {
    parkingOneFree = 1;
    Serial.print(parkingOneFree);
    Serial.print("\t");
  }
  else {
    parkingOneFree = 0;
    Serial.print(parkingOneFree);
    Serial.print("\t");
  }
  if (distance2 > 50) {
    parkingTwoFree = 1;
    Serial.print(parkingTwoFree);
  }
  else {
    parkingTwoFree = 0;
    Serial.print(parkingTwoFree);
  }
  Serial.print("\n");
}

