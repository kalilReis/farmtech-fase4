#include <DHTesp.h>

// Pin definitions
#define DHT_PIN   15   // DHT22 data pin
#define LED_PIN   19   // Status LED
#define RELAY_PIN 4    // Relay input
#define P_PIN     18   // Phosphorus button
#define K_PIN     5    // Potassium button
#define LDR_PIN   34   // LDR analog input (pH simulation)

#include <LiquidCrystal_I2C.h>

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C address 0x27, 16 columns, 2 rows

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(P_PIN, INPUT_PULLUP);
  pinMode(K_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FarmTech Fase 4");
  delay(2000);
  lcd.clear();
}

void loop() {
    // Read sensors
  // Optimization: Use 'float' for humidity as it requires decimal precision.
  // Use 'uint16_t' for ldrValue since analogRead returns 0-4095 on ESP32 (12-bit ADC),
  // which fits in 16 bits, saving memory compared to 'int' (usually 32-bit on ESP32).
  // Use 'bool' for hasP and hasK as they represent binary states.
  float humidity = dht.getHumidity();
  uint16_t ldrValue = analogRead(LDR_PIN);
  bool hasP = digitalRead(P_PIN) == LOW;  // pressed = presence
  bool hasK = digitalRead(K_PIN) == LOW;

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
    lcd.setCursor(0, 0);
    lcd.print("Irrigando...     ");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println("NÃO IRRIGANDO: Condições não atendidas");
    lcd.setCursor(0, 0);
    lcd.print("Nao irrigando   ");
  }

  // Display sensor values on LCD
  lcd.setCursor(0, 1);
  lcd.print("U:");
  lcd.print(humidity, 1);
  lcd.print("% P:");
  lcd.print(hasP ? "Y" : "N");
  lcd.print(" K:");
  lcd.print(hasK ? "Y" : "N");

  delay(2000);  // Wait 2 seconds between readings
}
