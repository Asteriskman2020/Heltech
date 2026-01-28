/*
 * Heltec Wireless Paper - NTP Time Display
 *
 * Displays current time from NTP server on the e-paper display
 * Board: Heltec Wireless Paper (ESP32-S3 + E-Paper + LoRa)
 *
 * Features:
 * - Connects to WiFi
 * - Syncs time via NTP
 * - Displays time on e-paper
 * - Updates every minute to save power
 */

#include <WiFi.h>
#include <time.h>
#include "HT_DEPG0213BNS800.h"  // Heltec e-paper library

// WiFi credentials - CHANGE THESE
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // GMT+7 for Thailand (adjust for your timezone)
const int daylightOffset_sec = 0;      // No daylight saving

// E-Paper display object
DEPG0213BNS800 display;

// Update interval (milliseconds)
const unsigned long UPDATE_INTERVAL = 60000;  // 1 minute
unsigned long lastUpdate = 0;

// Time structure
struct tm timeinfo;
char timeStr[32];
char dateStr[32];

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("================================");
  Serial.println("Heltec Wireless Paper NTP Clock");
  Serial.println("================================");

  // Initialize e-paper display
  Serial.println("Initializing e-paper display...");
  display.init();
  display.setFont(ArialMT_Plain_16);

  // Show connecting message
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(122/2, 20, "Connecting...");
  display.display();

  // Connect to WiFi
  connectWiFi();

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for NTP time sync...");

  // Wait for time sync
  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 10) {
    Serial.print(".");
    delay(1000);
    retry++;
  }
  Serial.println();

  if (retry >= 10) {
    Serial.println("Failed to get NTP time!");
    displayError("NTP Sync Failed");
  } else {
    Serial.println("Time synchronized!");
    updateDisplay();
  }
}

void loop() {
  // Update display every minute
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();

    // Re-sync time
    if (getLocalTime(&timeinfo)) {
      updateDisplay();
    } else {
      Serial.println("Failed to get time, reconnecting...");
      if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
      }
    }
  }

  // Print time to serial every second
  static unsigned long lastSerial = 0;
  if (millis() - lastSerial >= 1000) {
    lastSerial = millis();
    if (getLocalTime(&timeinfo)) {
      Serial.printf("%02d:%02d:%02d\n",
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);
    }
  }

  delay(100);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    displayError("WiFi Failed");
  }
}

void updateDisplay() {
  // Format time string (HH:MM)
  sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

  // Format date string
  const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  sprintf(dateStr, "%s, %s %d", weekdays[timeinfo.tm_wday], months[timeinfo.tm_mon], timeinfo.tm_mday);

  // Clear and draw
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  // Draw time (large)
  display.setFont(ArialMT_Plain_24);
  display.drawString(122/2, 5, timeStr);

  // Draw seconds
  char secStr[8];
  sprintf(secStr, ":%02d", timeinfo.tm_sec);
  display.setFont(ArialMT_Plain_16);
  display.drawString(122/2 + 45, 8, secStr);

  // Draw date
  display.setFont(ArialMT_Plain_10);
  display.drawString(122/2, 35, dateStr);

  // Draw year
  char yearStr[8];
  sprintf(yearStr, "%d", timeinfo.tm_year + 1900);
  display.drawString(122/2, 48, yearStr);

  // Draw WiFi indicator
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 0, "WiFi");
  }

  // Update e-paper
  display.display();

  Serial.printf("Display updated: %s - %s %d\n", timeStr, dateStr, timeinfo.tm_year + 1900);
}

void displayError(const char* message) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(122/2, 20, "Error:");
  display.setFont(ArialMT_Plain_10);
  display.drawString(122/2, 40, message);
  display.display();
}
