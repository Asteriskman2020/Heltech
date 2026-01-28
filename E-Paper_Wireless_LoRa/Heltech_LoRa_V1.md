# Heltec Wireless Paper - NTP Clock

This project displays the current time from an NTP server on the Heltec Wireless Paper's e-paper display.

## Hardware

### Board Specifications
- **MCU:** ESP32-S3 (dual-core, 240MHz)
- **Display:** 2.13" E-Paper (DEPG0213BNS800), 250x122 pixels
- **Wireless:** WiFi 802.11 b/g/n, LoRa SX1262
- **Flash:** 8MB
- **PSRAM:** 8MB

### Features Used
- WiFi for NTP time sync
- E-Paper display for low-power time display

## Software

### Dependencies
- Arduino IDE or PlatformIO
- Heltec ESP32 Board Support Package

### Installation

1. **Add Heltec board support to Arduino IDE:**
   - Go to File > Preferences
   - Add to Additional Board Manager URLs:
     ```
     https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
     ```
   - Go to Tools > Board > Board Manager
   - Search for "Heltec ESP32" and install

2. **Select your board:**
   - Tools > Board > Heltec ESP32 Series Dev-boards > Wireless Paper

3. **Configure WiFi:**
   Edit the sketch and change:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

4. **Set your timezone:**
   ```cpp
   const long gmtOffset_sec = 7 * 3600;  // GMT+7 for Thailand
   ```
   Common offsets:
   | Timezone | Offset |
   |----------|--------|
   | UTC | 0 |
   | EST (US) | -5 * 3600 |
   | PST (US) | -8 * 3600 |
   | CET (Europe) | 1 * 3600 |
   | JST (Japan) | 9 * 3600 |
   | ICT (Thailand) | 7 * 3600 |

5. **Upload the sketch**

## Display Output

The e-paper display shows:
```
     WiFi
      14:30:45
   Wednesday, Jan 28
        2026
```

- **Time:** Large format HH:MM:SS
- **Date:** Day of week, Month Day
- **Year:** Current year
- **WiFi indicator:** Shows connection status

## Serial Output

At 115200 baud:
```
================================
Heltec Wireless Paper NTP Clock
================================
Initializing e-paper display...
Connecting to WiFi: YourNetwork
.....
Connected! IP: 192.168.1.100
Waiting for NTP time sync...
Time synchronized!
Display updated: 14:30 - Wednesday, Jan 28 2026
14:30:45
14:30:46
...
```

## Power Considerations

- E-paper only uses power during refresh
- Display updates once per minute to conserve power
- For battery operation, consider:
  - Using deep sleep between updates
  - Reducing update frequency
  - Disconnecting WiFi after sync

## Customization

### Change Update Interval
```cpp
const unsigned long UPDATE_INTERVAL = 60000;  // milliseconds
```

### Change NTP Server
```cpp
const char* ntpServer = "pool.ntp.org";
// Alternatives:
// "time.google.com"
// "time.nist.gov"
// "asia.pool.ntp.org"
```

## Troubleshooting

1. **WiFi won't connect:**
   - Verify SSID and password
   - Check if 2.4GHz network (5GHz not supported)
   - Move closer to router

2. **Time shows wrong:**
   - Adjust `gmtOffset_sec` for your timezone
   - Check `daylightOffset_sec` if DST applies

3. **Display not updating:**
   - E-paper may need full refresh
   - Check serial output for errors

4. **Upload fails:**
   - Hold BOOT button while uploading
   - Check COM port selection

## Pin Reference

| Function | GPIO |
|----------|------|
| E-Paper BUSY | 7 |
| E-Paper RST | 6 |
| E-Paper DC | 5 |
| E-Paper CS | 4 |
| E-Paper CLK | 3 |
| E-Paper DIN | 2 |
| LoRa NSS | 8 |
| LoRa RST | 12 |
| LoRa DIO1 | 14 |

## License

MIT License
