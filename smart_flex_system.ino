/*
  Smart Flex System - Sign Language to Text
  -----------------------------------------
  IEEM Mini Project | AISSMS COE, SE R&A, Batch-C
  Guide: R. Wani Sir

  Description:
    Converts finger bend gestures (with flex sensor) into text.
    Displays result on LCD and transmits via Bluetooth.
    Provides alert (buzzer) for emergency detection.

  Hardware:
    - Arduino Uno
    - Flex Sensor (A0)
    - 16x2 I2C LCD
    - HC-05 Bluetooth Module (pins 10, 11)
    - Buzzer (D9)

  Developed by: AISSMS COE SE R&A Team
  Date: November 11, 2025
  Version: 8.1
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>


// --- Pins ---
const int flexPin = A0;
const int btRx = 10;
const int btTx = 11;
const int buzzerPin = 9; // 🔔 Buzzer pin


// --- Objects ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial Bluetooth(btRx, btTx);


// --- Calibration & Config ---
int sensorMin = 1023, sensorMax = 0;
bool isCalibrating = false;
int helpThreshold = 75, updateDelay = 300;
bool echoEnabled = true;
bool buzzerEnabled = true;


// EEPROM Addresses
const int addrMin = 0, addrMax = 4;


// --- Banner ---
const char bannerText[] = " SMART FLEX SYSTEM ";
int bannerIndex = 0;
unsigned long lastScrollTime = 0;
int scrollInterval = 300;
bool helpAlertActive = false;


// ----------------------------------------------------
// --- Utility: Beep patterns ---
// ----------------------------------------------------
void beep(int duration) {
  if (!buzzerEnabled) return;
  tone(buzzerPin, 1000, duration);
  delay(duration + 20);
  noTone(buzzerPin);
}


void doubleBeep() {
  if (!buzzerEnabled) return;
  beep(150);
  delay(100);
  beep(150);
}


// ----------------------------------------------------
// --- Welcome Message ---
// ----------------------------------------------------
void showSimpleWelcome() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AISSMS COE");
  lcd.setCursor(2, 1);
  lcd.print("SE RA");
  delay(1500);

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("IEEM Project");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sign Language To");
  lcd.setCursor(4, 1);
  lcd.print("Text");
  delay(2000);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Initializing...");
  delay(1500);
  lcd.clear();
}


// ----------------------------------------------------
// --- Setup ---
// ----------------------------------------------------
void setup() {
  Serial.begin(9600);
  Bluetooth.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);

  showSimpleWelcome();
  beep(150); // Startup beep

  Serial.println("=== SMART FLEX SYSTEM STARTED ===");
  Bluetooth.println("=== SMART FLEX SYSTEM v8.1 ===");
  Bluetooth.println("Type HELP for commands");

  loadCalibration();
  if (sensorMin == 1023 || sensorMax == 0 || (sensorMax - sensorMin) < 30) {
    Bluetooth.println("⚠️ No valid calibration found. Starting auto-calibration...");
    autoCalibrate();
  }

  showReadyScreen();
}


// ----------------------------------------------------
// --- Main Loop ---
// ----------------------------------------------------
void loop() {
  // --- Banner Scrolling ---
  if (millis() - lastScrollTime > scrollInterval) {
    lastScrollTime = millis();
    char buffer[17];
    for (int i = 0; i < 16; i++) {
      buffer[i] = bannerText[(bannerIndex + i) % (sizeof(bannerText) - 1)];
    }
    buffer[16] = '\0';
    lcd.setCursor(0, 0);
    lcd.print(buffer);
    bannerIndex = (bannerIndex + 1) % (sizeof(bannerText) - 1);
  }

  if (Bluetooth.available()) handleCommand();
  if (isCalibrating) return;

  // --- Sensor Reading ---
  long total = 0;
  for (int i = 0; i < 10; i++) {
    total += analogRead(flexPin);
    delay(5);
  }
  int sensorValue = total / 10;

  if (sensorValue < sensorMin) sensorValue = sensorMin;
  if (sensorValue > sensorMax) sensorValue = sensorMax;

  int rawRange = sensorMax - sensorMin;
  int bendPercent;
  if (rawRange < 150)
    bendPercent = (int)(((float)(sensorValue - sensorMin) / (float)rawRange) * 150);
  else
    bendPercent = map(sensorValue, sensorMin, sensorMax, 0, 100);

  bendPercent = constrain(bendPercent, 0, 100);
  const char* message = (bendPercent < helpThreshold) ? "I am Fine." : "Need Help!";

  // --- Buzzer Alert ---
  if (bendPercent >= helpThreshold && !helpAlertActive) {
    doubleBeep();
    helpAlertActive = true;
  } else if (bendPercent < helpThreshold) {
    helpAlertActive = false;
  }

  // --- Output ---
  if (echoEnabled) {
    Serial.print("Bend: ");
    Serial.print(bendPercent);
    Serial.print("% | Raw: ");
    Serial.print(sensorValue);
    Serial.print(" | ");
    Serial.println(message);

    Bluetooth.print("Bend: ");
    Bluetooth.print(bendPercent);
    Bluetooth.print("% | ");
    Bluetooth.println(message);
  }

  lcd.setCursor(0, 1);
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", message);
  lcd.print(buf);

  delay(updateDelay);
}


// ----------------------------------------------------
// --- Calibration with Live Display ---
// ----------------------------------------------------
void autoCalibrate() {
  isCalibrating = true;
  lcd.clear();
  Bluetooth.println("--- MANUAL CALIBRATION START ---");

  // Step 1: Straight
  lcd.setCursor(0, 0);
  lcd.print("Keep STRAIGHT...");
  lcd.setCursor(0, 1);
  lcd.print("Live: ");
  delay(1000);

  long straightTotal = 0;
  int samples = 0;
  unsigned long start = millis();
  while (millis() - start < 5000) {
    int val = analogRead(flexPin);
    straightTotal += val;
    samples++;
    lcd.setCursor(6, 1);
    lcd.print("    ");
    lcd.setCursor(6, 1);
    lcd.print(val);
    Bluetooth.print("Straight ADC: ");
    Bluetooth.println(val);
    delay(200);
  }
  sensorMin = straightTotal / samples;

  // Step 2: Bent
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Now BEND Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Live: ");
  delay(1000);

  long bentTotal = 0;
  samples = 0;
  start = millis();
  while (millis() - start < 5000) {
    int val = analogRead(flexPin);
    bentTotal += val;
    samples++;
    lcd.setCursor(6, 1);
    lcd.print("    ");
    lcd.setCursor(6, 1);
    lcd.print(val);
    Bluetooth.print("Bent ADC: ");
    Bluetooth.println(val);
    delay(200);
  }
  sensorMax = bentTotal / samples;

  // Range check
  int range = abs(sensorMax - sensorMin);
  if (range < 30) {
    Bluetooth.println("⚠️ Flex range too small. Expanding...");
    sensorMin -= 30;
    sensorMax += 30;
    if (sensorMin < 0) sensorMin = 0;
    if (sensorMax > 1023) sensorMax = 1023;
  }
  if (sensorMax < sensorMin) {
    int tmp = sensorMin;
    sensorMin = sensorMax;
    sensorMax = tmp;
  }

  saveCalibration();

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Calib Complete!");
  lcd.setCursor(0, 1);
  lcd.print("Range:");
  lcd.print(sensorMax - sensorMin);
  lcd.print(" ADC");
  beep(200); // ✅ Beep after calibration
  delay(2500);

  showReadyScreen();
  Bluetooth.println("Calibration Complete!");
  isCalibrating = false;
}


// ----------------------------------------------------
void saveCalibration() {
  EEPROM.put(addrMin, sensorMin);
  EEPROM.put(addrMax, sensorMax);
}
void loadCalibration() {
  EEPROM.get(addrMin, sensorMin);
  EEPROM.get(addrMax, sensorMax);
  if (sensorMin < 0 || sensorMax > 1023 || (sensorMax - sensorMin) < 30) {
    Bluetooth.println("Bad EEPROM data, restoring defaults...");
    sensorMin = 200;
    sensorMax = 900;
    saveCalibration();
  }
}


// ----------------------------------------------------
// --- Terminal Commands (Essential Only) ---
// ----------------------------------------------------
void handleCommand() {
  char cmd[32];
  int i = 0;
  while (Bluetooth.available() && i < 31) {
    char c = Bluetooth.read();
    if (c == '\n' || c == '\r') break;
    cmd[i++] = c;
  }
  cmd[i] = '\0';

  if (strcasecmp(cmd, "CAL") == 0) autoCalibrate();
  else if (strcasecmp(cmd, "STATUS") == 0) showStatus();
  else if (strcasecmp(cmd, "RESET") == 0) resetCalibration();
  else if (strcasecmp(cmd, "HELP") == 0) showHelp();
  else if (strcasecmp(cmd, "ECHOON") == 0) { echoEnabled = true; Bluetooth.println("Echo enabled."); }
  else if (strcasecmp(cmd, "ECHOOFF") == 0) { echoEnabled = false; Bluetooth.println("Echo disabled."); }
  else if (strcasecmp(cmd, "BUZZON") == 0) { buzzerEnabled = true; Bluetooth.println("Buzzer enabled."); beep(100); }
  else if (strcasecmp(cmd, "BUZZOFF") == 0) { buzzerEnabled = false; Bluetooth.println("Buzzer disabled."); noTone(buzzerPin); }
  else if (strcasecmp(cmd, "INFO") == 0) showInfo();
  else Bluetooth.println("Unknown command. Type HELP.");
}


void showHelp() {
  Bluetooth.println("=== COMMANDS ===");
  Bluetooth.println("CAL, STATUS, RESET");
  Bluetooth.println("ECHOON/ECHOOFF, BUZZON/BUZZOFF");
  Bluetooth.println("INFO");
}


void showInfo() {
  Bluetooth.println("Smart Flex System v8.1");
  Bluetooth.println("AISSMS COE, Pune | SE R&A Batch-C");
  Bluetooth.println("IEEM Project | Guide: R. Wani Sir");
  Bluetooth.println("Build: " __DATE__ " " __TIME__);
}


// ----------------------------------------------------
void resetCalibration() {
  sensorMin = 1023;
  sensorMax = 0;
  saveCalibration();
  Bluetooth.println("Calibration reset. Run CAL.");
  lcd.clear();
  lcd.print("Calibration Reset");
  delay(1500);
  showReadyScreen();
}


void showReadyScreen() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("System Ready");
  lcd.setCursor(1, 1);
  lcd.print("Type HELP in App");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("I am Fine.       ");
}


void showStatus() {
  Bluetooth.println("=== SENSOR STATUS ===");
  Bluetooth.print("Min: "); Bluetooth.print(sensorMin);
  Bluetooth.print(" | Max: "); Bluetooth.println(sensorMax);
  Bluetooth.print("Range: "); Bluetooth.println(sensorMax - sensorMin);
  Bluetooth.print("Help Threshold: "); Bluetooth.println(helpThreshold);
}
