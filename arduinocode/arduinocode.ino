#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

String input = "";

unsigned long lastBlink = 0;
unsigned long lastDataTime = 0;

int blinkDelay = 500;
int connectionTimeout = 3000; // 3 seconds without data = disconnected

bool showLastDot = true;
bool dataReceived = false;

String cpu = "";
String ram = "";
String ping = "";

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  showWaitingScreen();
}

void loop() {
  // If connected but no new data for 3 seconds, go back to waiting
  if (dataReceived && millis() - lastDataTime > connectionTimeout) {
    dataReceived = false;
    cpu = "";
    ram = "";
    ping = "";
    showWaitingScreen();
  }

  if (!dataReceived) {
    waitingAnimation();
  }

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      if (input.length() > 0) {
        dataReceived = true;
        lastDataTime = millis();
        parseData(input);
        input = "";
      }
    } else {
      input += c;
    }
  }
}

void showWaitingScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("connection...");
}

void waitingAnimation() {
  if (millis() - lastBlink >= blinkDelay) {
    lastBlink = millis();
    showLastDot = !showLastDot;

    lcd.setCursor(0, 0);
    lcd.print("Waiting for     ");

    lcd.setCursor(0, 1);

    if (showLastDot) {
      lcd.print("connection...");
    } else {
      lcd.print("connection.. ");
    }
  }
}

void parseData(String data) {
  // Expected format:
  // CPU:45%,RAM:62%,WIFI:MyWifi,PING:24ms

  int cpuIndex = data.indexOf("CPU:");
  int ramIndex = data.indexOf(",RAM:");
  int wifiIndex = data.indexOf(",WIFI:");
  int pingIndex = data.indexOf(",PING:");

  if (cpuIndex == -1 || ramIndex == -1 || wifiIndex == -1 || pingIndex == -1) return;

  cpu = data.substring(cpuIndex + 4, ramIndex);
  ram = data.substring(ramIndex + 5, wifiIndex);
  ping = data.substring(pingIndex + 6);

  updateLCD();
}

void updateLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("CPU:");
  lcd.print(cpu);
  lcd.print(" RAM:");
  lcd.print(ram);

  lcd.setCursor(0, 1);
  lcd.print("WIFI Ping:");
  lcd.print(ping);
}