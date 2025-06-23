#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
#define DHT_PIN   15   // DHT22 data pin
#define LED_PIN   19   // Status LED
#define RELAY_PIN 4    // Relay input
#define P_PIN     18   // Phosphorus button
#define K_PIN     5    // Potassium button
#define LDR_PIN   34   // LDR analog input (pH simulation)
#define SOIL_SENSOR_PIN 34 // Soil sensor analog input (matches Wokwi diagram)

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
  float humidity = dht.getHumidity();
  uint16_t ldrValue = analogRead(LDR_PIN);
  uint16_t soilMoisture = analogRead(SOIL_SENSOR_PIN);
  bool hasP = digitalRead(P_PIN) == LOW;  // pressed = presence
  bool hasK = digitalRead(K_PIN) == LOW;

  // Determine daylight status based on LDR value
  const char* daylightStatus;
  if (ldrValue > 3000) {
    daylightStatus = "Night";
  } else if (ldrValue > 2000) {
    daylightStatus = "Morning";
  } else {
    daylightStatus = "Noon";
  }

  // Print readings
  Serial.printf(
    "Umid: %.1f%% | LDR: %d (%s) | SoilMoist: %d | P: %d | K: %d\n",
    humidity, ldrValue, daylightStatus, soilMoisture, hasP, hasK
  );

  // Irrigation logic:
  bool ldrValid = (ldrValue > 1000 && ldrValue < 3000);
  bool soilMoistureValid = (soilMoisture < 2000); // Example threshold, adjust as needed

  if (humidity < 40.0 && soilMoistureValid && (strcmp(daylightStatus, "Morning") == 0 || strcmp(daylightStatus, "Night") == 0)) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("IRRIGANDO: Condições atendidas (umidade < 40%, solo seco, manhã ou noite)");
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

  delay(2000); // Keep status visible for 2 seconds

  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear line

  lcd.setCursor(0, 1);
  lcd.print(daylightStatus);

  delay(1000); // Delay before showing soil moisture

  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.print(soilMoisture);

  delay(1000); // Delay before showing LDR

  lcd.setCursor(0, 1);
  lcd.print("    "); // Clear part of line
  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(ldrValue);

  delay(2000);  // Wait 2 seconds between readings
}
