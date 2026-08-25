#define PIR_PIN 27
#define GREEN_LED 25

#define MQ2_PIN 34
#define RED_LED 26
#define BUZZER 14

#define GAS_THRESHOLD 2000

volatile bool motionDetected = false;

void IRAM_ATTR motionISR() {
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(MQ2_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  attachInterrupt(
    digitalPinToInterrupt(PIR_PIN),
    motionISR,
    RISING
  );

  Serial.println("Smart Home Security System Started");
}

void loop() {

  int gasValue = analogRead(MQ2_PIN);

  Serial.print("Gas Level: ");
  Serial.println(gasValue);

  // SAFETY OVERRIDE
  if (gasValue > GAS_THRESHOLD) {

    Serial.println("!!! GAS/SMOKE DETECTED - SAFETY OVERRIDE !!!");

    // Turn OFF normal lighting
    digitalWrite(GREEN_LED, LOW);

    // Activate alarm
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);

    // Ignore motion while danger exists
    motionDetected = false;
  }

  // NORMAL OPERATION
  else {

    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    if (motionDetected) {

      Serial.println("Motion Detected!");

      digitalWrite(GREEN_LED, HIGH);

      delay(3000);

      digitalWrite(GREEN_LED, LOW);

      motionDetected = false;
    }
  }

  delay(200);
}
