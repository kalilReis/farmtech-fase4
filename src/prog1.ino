#include <DHTesp.h>

// Pin definitions
#define DHT_PIN   15   // DHT22 data pin
#define LED_PIN   19   // Status LED
#define RELAY_PIN 4    // Relay input
#define P_PIN     18   // Phosphorus button
#define K_PIN     5    // Potassium button
#define LDR_PIN   34   // LDR analog input (pH simulation)

DHTesp dht;

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(P_PIN, INPUT_PULLUP);
  pinMode(K_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
}

void loop() {
  // Read sensors
  float humidity = dht.getHumidity();
  int   ldrValue = analogRead(LDR_PIN);
  bool  hasP     = digitalRead(P_PIN) == LOW;  // pressed = presence
  bool  hasK     = digitalRead(K_PIN) == LOW;

  // Print readings
  Serial.printf(
    "Umid: %.1f%% | pH(sim): %d | P: %d | K: %d\n",
    humidity, ldrValue, hasP, hasK
  );

  // Irrigation logic:
  // If humidity < 40% AND (phosphorus OR potassium present) AND LDR (pH) in valid range → irrigate
  // Example: LDR value between 1000 and 3000 (adjust as needed for your simulation)
  bool ldrValid = (ldrValue > 1000 && ldrValue < 3000);

  if (humidity < 40.0 && (hasP || hasK) && ldrValid) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("IRRIGANDO: Condições atendidas (umidade < 40%, P ou K presente, pH válido)");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println("NÃO IRRIGANDO: Condições não atendidas");
  }

  delay(2000);  // Wait 2 seconds between readings
}
