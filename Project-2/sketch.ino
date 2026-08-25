const int moisturePin = A0;
const int relayPin = 7;

const int dryThreshold = 500;

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);

  // Pump OFF initially
  digitalWrite(relayPin, LOW);
}

void loop() {

  // Read soil moisture sensor
  int moistureValue = analogRead(moisturePin);

  Serial.print("Moisture Value: ");
  Serial.println(moistureValue);

  // Check whether soil is dry
  if (moistureValue < dryThreshold) {

    digitalWrite(relayPin, HIGH);

    Serial.println("Soil is DRY -> Pump ON");

  } else {

    digitalWrite(relayPin, LOW);

    Serial.println("Soil is MOIST -> Pump OFF");
  }

  delay(1000);
}
