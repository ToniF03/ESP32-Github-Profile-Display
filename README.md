# ESP32 GitHub Profile Display

A beautiful e-paper display that shows your GitHub contribution statistics on a 7.5" e-ink screen. The ESP32 fetches your GitHub profile data via WiFi and renders an elegant dashboard with contribution graphs, streaks, and statistics.

![Project Preview](preview.jpg) <!-- Add your project photo here -->

## Features

- 📊 **Contribution Heatmap** - Visual 53-week contribution calendar with grayscale dithering
- 🔥 **Streak Tracking** - Current and longest contribution streaks
- 📈 **Statistics Display** - Total contributions, max daily contributions, and averages
- 🎨 **Grayscale Rendering** - Bayer dithering for smooth grayscale on black/white e-paper
- 📡 **WiFi Connectivity** - Auto-connects to WiFi with signal strength display
- 🔋 **Power Efficient** - Deep sleep mode for 6 hours between updates
- ⏰ **NTP Time Sync** - Accurate time display with automatic timezone support

## Hardware Requirements

- **ESP32 Development Board** (ESP32-DevKitC or similar)
- **7.5" E-Paper Display** - GDEY075T7 (800x480, Black/White)
- **Connecting Wires**

### Pin Configuration

| E-Paper Pin | ESP32 Pin | Description |
|-------------|-----------|-------------|
| BSY         | GPIO 4    | Busy signal |
| RST         | GPIO 16   | Reset       |
| DC          | GPIO 17   | Data/Command|
| CS          | GPIO 5    | Chip Select |
| SCK         | GPIO 18   | SPI Clock   |
| SDI (MOSI)  | GPIO 23   | SPI Data    |

## Software Requirements

- [PlatformIO](https://platformio.org/) IDE or VS Code with PlatformIO extension
- Arduino framework for ESP32
- Required libraries (auto-installed via PlatformIO):
  - `GxEPD2` (^1.6.5) - E-paper display driver
  - `ArduinoJson` (^7.4.2) - JSON parsing for API responses

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/ToniF03/ESP32-Github-Profile-Display.git
cd ESP32-Github-Profile-Display
```

### 2. Configure Credentials

Edit `include/resources/credentials.h` with your information:

```cpp
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"
#define GITHUB_USERNAME "YourGitHubUsername"
#define GITHUB_PAT "your_github_personal_access_token"
```

**⚠️ IMPORTANT: Never commit this file with real credentials!**

#### Getting a GitHub Personal Access Token

1. Go to [GitHub Settings > Developer settings > Personal access tokens](https://github.com/settings/tokens)
2. Click "Generate new token (classic)"
3. Give it a descriptive name (e.g., "ESP32 Display")
4. Select the `read:user` scope (to read profile information)
5. Click "Generate token" and copy it immediately

### 3. Build and Upload

Using PlatformIO:

```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

Or use the PlatformIO IDE buttons in VS Code.

## How It Works

### Data Flow

1. **Wake Up** - ESP32 wakes from deep sleep
2. **WiFi Connection** - Connects to configured WiFi network (30s timeout)
3. **Time Sync** - Gets current time from NTP server
4. **API Calls** - Fetches data from GitHub:
   - User profile (followers, following, name)
   - Contribution calendar (last 365 days)
5. **Data Processing** - Calculates statistics:
   - Total contributions
   - Current streak
   - Longest streak
   - Max contributions in a day
   - Average contributions per day
6. **Rendering** - Draws everything on e-paper using Bayer dithering
7. **Deep Sleep** - Sleeps for 6 hours to save power

### Grayscale Rendering

The display uses a **Bayer 4x4 dithering matrix** to simulate 18 levels of grayscale on the black/white e-paper display. This creates smooth gradients in the contribution heatmap.

## Display Layout

```
┌─────────────────────────────────────────────────────────────┐
│  Total Contributions                                         │
│      XXXX                                                     │
│      Contributions in the last year                          │
│                                                               │
│  ┌────────────┐  ┌────────────┐                             │
│  │    XX      │  │    XX      │                             │
│  │ Longest    │  │   Most     │                             │
│  │ Streak     │  │  in a Day  │                             │
│  └────────────┘  └────────────┘                             │
│                                                               │
│  ┌────────────┐  ┌────────────┐                             │
│  │    XX      │  │   XX.XX    │                             │
│  │  Current   │  │  Average   │                             │
│  │  Streak    │  │  per Day   │                             │
│  └────────────┘  └────────────┘                             │
│                                                               │
│  [Contribution Heatmap - 53 weeks x 7 days]                 │
│  ████████████████████████████████████████████████████████   │
│  ████████████████████████████████████████████████████████   │
│  ████████████████████████████████████████████████████████   │
│                                                               │
│  ⚡ username (Full Name)    📡 WiFi Signal    🕐 Time        │
└─────────────────────────────────────────────────────────────┘
```

## Configuration

### Update Interval

The deep sleep duration is set in `main.cpp`:

```cpp
// Current: 6 hours (21,600,000,000 microseconds)
esp_sleep_enable_timer_wakeup(2.16e10);
```

Modify this value to change update frequency:
- 1 hour: `3.6e9`
- 12 hours: `4.32e10`
- 24 hours: `8.64e10`

### Timezone

Adjust the timezone in `main.cpp`:

```cpp
// GMT+1 with DST offset of 1 hour
configTime(3600, 3600, "pool.ntp.org");
```

Change the first parameter for different timezones (offset in seconds).

## Troubleshooting

### WiFi Connection Failed

- Check SSID and password in `credentials.h`
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Move closer to router
- Check serial monitor for detailed error messages

### Display Not Updating

- Verify all pins are correctly connected
- Check if BSY pin is working (should go low when display is ready)
- Ensure adequate power supply (USB power should work)

### API Errors

- Verify GitHub PAT has correct permissions
- Check if token has expired
- Ensure username is correct
- Monitor serial output for HTTP error codes

### Compilation Errors

```bash
# Clean build files
pio run --target clean

# Update libraries
pio lib update

# Rebuild
pio run
```

## Power Consumption

- **Active (WiFi + Display Update)**: ~150mA for ~30 seconds
- **Deep Sleep**: ~10µA
- **Average over 6 hours**: < 1mA

With a 1000mAh battery, the device can run for several months between charges.

## Future Enhancements

- [ ] Add follower/following count display
- [ ] Show repository count and stars
- [ ] Battery level indicator
- [ ] Web configuration portal

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is open source and available under the [MIT License](LICENSE).

## Acknowledgments

- [GxEPD2](https://github.com/ZinggJM/GxEPD2) - Excellent e-paper library
- [ArduinoJson](https://arduinojson.org/) - Efficient JSON parsing
- GitHub GraphQL API for contribution data
- Roboto font family

## Author

**ToniF03**
- GitHub: [@ToniF03](https://github.com/ToniF03)

## Support

If you find this project helpful, please give it a ⭐ on GitHub!

---

**Note**: Remember to keep your `credentials.h` file private and never commit it to a public repository. Consider adding it to `.gitignore`.
