/*
 * Heltec Wireless Paper - NTP Time Display
 *
 * Displays current time from NTP server on the e-paper display
 * Board: Heltec Wireless Paper (ESP32-S3 + E-Paper + LoRa)
 */

#include <WiFi.h>
#include <time.h>
#include "HT_QYEG0213RWS800_BWR.h"

// WiFi credentials - CHANGE THESE
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // GMT+7 for Thailand
const int daylightOffset_sec = 0;

// E-Paper display for Wireless Paper
// Pins: RST=6, DC=5, CS=4, BUSY=7, SCK=3, MOSI=2
QYEG0213RWS800_BWR display(6, 5, 4, 7, 3, 2, -1, 6000000);

// Update interval
const unsigned long UPDATE_INTERVAL = 60000;
unsigned long lastUpdate = 0;

struct tm timeinfo;

void VextON(void) {
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);
}

void VextOFF(void) {
  pinMode(45, OUTPUT);
  digitalWrite(45, HIGH);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n================================");
  Serial.println("Heltec Wireless Paper NTP Clock");
  Serial.println("================================\n");

  VextON();
  delay(100);

  display.init();

  display.clear();
  display.update(BLACK_BUFFER);
  display.drawString(10, 50, "Connecting WiFi...");
  display.update(BLACK_BUFFER);
  display.display();

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

    display.clear();
    display.update(BLACK_BUFFER);
    display.drawString(10, 50, "WiFi OK, syncing NTP...");
    display.update(BLACK_BUFFER);
    display.display();

    // Initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    int retry = 0;
    while (!getLocalTime(&timeinfo) && retry < 10) {
      delay(1000);
      retry++;
    }

    if (retry < 10) {
      Serial.println("Time synchronized!");
      updateDisplay();
    } else {
      showError("NTP Failed");
    }
  } else {
    showError("WiFi Failed");
  }
}

void loop() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    if (getLocalTime(&timeinfo)) {
      updateDisplay();
    }
  }

  // Print to serial every second
  static unsigned long lastSerial = 0;
  if (millis() - lastSerial >= 1000) {
    lastSerial = millis();
    if (getLocalTime(&timeinfo)) {
      Serial.printf("%02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
  }

  delay(100);
}

void updateDisplay() {
  const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char timeBuf[16];
  char dateBuf[48];

  sprintf(timeBuf, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  sprintf(dateBuf, "%s, %s %d, %d", weekdays[timeinfo.tm_wday], months[timeinfo.tm_mon],
          timeinfo.tm_mday, timeinfo.tm_year + 1900);

  display.clear();
  display.update(BLACK_BUFFER);

  // WiFi indicator (top left)
  display.drawString(5, 5, WiFi.status() == WL_CONNECTED ? "WiFi" : "----");

  // Time (large, centered) - display is 250x122
  display.drawString(60, 40, timeBuf);

  // Separator line
  display.drawHorizontalLine(20, 75, 210);

  // Date (below separator)
  display.drawString(30, 85, dateBuf);

  display.update(BLACK_BUFFER);
  display.display();

  Serial.printf("Display updated: %s - %s\n", timeBuf, dateBuf);
}

void showError(const char* msg) {
  display.clear();
  display.update(BLACK_BUFFER);
  display.drawString(50, 40, "Error:");
  display.drawString(50, 60, msg);
  display.update(BLACK_BUFFER);
  display.display();
  Serial.printf("Error: %s\n", msg);
}
