# ESP32 GitHub Profile Display

A beautiful e-paper display that shows your GitHub contribution statistics on a 7.5" e-ink screen. The ESP32 fetches your GitHub profile data via WiFi and renders an elegant dashboard with a contribution heatmap, streaks, and detailed statistics—all with ultra-low power consumption thanks to deep sleep mode.

![Project Preview](preview.jpg)
*Add your project photo here to showcase the display*

## Features

- 📊 **Contribution Heatmap** - Visual 53-week contribution calendar with grayscale dithering
- 🔥 **Streak Tracking** - Current and longest contribution streaks
- 📈 **Statistics Display** - Total contributions, max daily contributions, and averages
- 🎨 **Grayscale Rendering** - Bayer dithering for smooth grayscale on black/white e-paper
- 📡 **WiFi Connectivity** - Auto-connects to WiFi with signal strength display
- 🔋 **Power Efficient** - Deep sleep mode for 6 hours between updates
- ⏰ **NTP Time Sync** - Accurate time display with automatic timezone support

## Hardware Requirements

### Components

- **ESP32 Development Board** (ESP32-DevKitC or compatible)
- **7.5" E-Paper Display** - Waveshare/Good Display GDEY075T7 (800x480 pixels, Black/White)
- **Jumper Wires** (Female-to-Female recommended)
- **Micro-USB Cable** (for programming and power)
- **Optional**: External power source (battery pack, solar panel, etc.)

### Wiring Diagram

| E-Paper Pin | ESP32 Pin | Pin Type    | Description           |
|-------------|-----------|-------------|-----------------------|
| VCC         | 3.3V      | Power       | Power supply (3.3V)   |
| GND         | GND       | Ground      | Ground                |
| BSY         | GPIO 4    | Input       | Busy signal           |
| RST         | GPIO 16   | Output      | Reset                 |
| DC          | GPIO 17   | Output      | Data/Command select   |
| CS          | GPIO 5    | Output      | Chip Select (SPI)     |
| SCK (CLK)   | GPIO 18   | Output      | SPI Clock             |
| SDI (MOSI)  | GPIO 23   | Output      | SPI Data (MOSI)       |

**Note**: Ensure your e-paper display is rated for 3.3V operation. Some displays require 5V.

## Software Requirements

### Development Environment

- [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
  - *OR* [PlatformIO IDE](https://platformio.org/) standalone
- Arduino framework for ESP32 (installed automatically by PlatformIO)

### Dependencies

The following libraries are automatically installed via PlatformIO:

- **GxEPD2** (v1.6.5) - E-paper display driver library
  - Provides hardware abstraction for various e-paper displays
  - Handles SPI communication and display refresh
- **ArduinoJson** (v7.4.2) - JSON parsing library
  - Parses GitHub API responses efficiently
  - Minimal memory footprint

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/ToniF03/ESP32-Github-Profile-Display.git
cd ESP32-Github-Profile-Display
```

### 2. Configure Credentials

Create or edit `include/resources/credentials.h` with your information:

```cpp
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"
#define GITHUB_USERNAME "YourGitHubUsername"
#define GITHUB_PAT "your_github_personal_access_token"
```

**⚠️ SECURITY WARNING**: 
- **Never commit this file with real credentials to a public repository!**
- Add `include/resources/credentials.h` to your `.gitignore`
- Consider creating a `credentials.h.example` template file instead

#### Getting a GitHub Personal Access Token (PAT)

1. Navigate to [GitHub Settings → Developer settings → Personal access tokens → Tokens (classic)](https://github.com/settings/tokens)
2. Click **"Generate new token (classic)"**
3. Configure the token:
   - **Note**: Give it a descriptive name (e.g., "ESP32 GitHub Display")
   - **Expiration**: Choose your preferred duration (90 days, 1 year, or no expiration)
   - **Scopes**: Select `read:user` (allows reading profile information)
4. Click **"Generate token"** at the bottom
5. **Important**: Copy the token immediately—you won't be able to see it again!
6. Store it securely in your `credentials.h` file

> **Note**: The project uses GitHub's GraphQL API to fetch contribution data, which requires authentication even for public profiles.

### 3. Build and Upload

#### Option A: Using PlatformIO CLI

```bash
# Build the project
pio run

# Upload to ESP32 (automatically detects port)
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Or do all in one command
pio run --target upload && pio device monitor
```

#### Option B: Using VS Code PlatformIO Extension

1. Open the project folder in VS Code
2. Click the **Build** button (checkmark) in the bottom toolbar
3. Click the **Upload** button (arrow) to flash the ESP32
4. Click the **Serial Monitor** button (plug) to view output

> **Tip**: Use Ctrl+C to exit the serial monitor

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

The display uses a **Bayer 4x4 dithering matrix** to simulate 18 levels of grayscale (0=white, 17=black) on the monochrome e-paper display. This ordered dithering algorithm creates smooth gradients in the contribution heatmap by varying the density of black pixels in a checkerboard-like pattern.

**Dithering Matrix:**
```
 0   8   2  10
12   4  14   6
 3  11   1   9
15   7  13   5
```

The contribution intensity is mapped to grayscale levels based on the maximum daily contributions, providing a visual representation of coding activity.

## Display Layout

The 800x480 pixel display is organized as follows:

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                   │
│   Total Contributions                                            │
│       XXXX                                                        │
│   Contributions in the last year                                 │
│                                                                   │
│   ┌────────────┐  ┌────────────┐  ┌────────────┐  ┌──────────┐ │
│   │    XX      │  │    XX      │  │    XX      │  │  XX.XX   │ │
│   │  Longest   │  │   Most     │  │  Current   │  │ Average  │ │
│   │  Streak    │  │  in a Day  │  │  Streak    │  │ per Day  │ │
│   └────────────┘  └────────────┘  └────────────┘  └──────────┘ │
│                                                                   │
│   ┌───────────────────────────────────────────────────────────┐ │
│   │  53-Week Contribution Heatmap (GitHub-style calendar)     │ │
│   │  ▓░░█▓░░░▓█░█░░░░░██▓░░░░░░░█▓░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  █▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  ░░░░░░░▓█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   │  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  │ │
│   └───────────────────────────────────────────────────────────┘ │
│                                                                   │
│  🐙 username (Full Name)  � Good (-62 dBm)  🕐 02/11/2025 14:30 │
└─────────────────────────────────────────────────────────────────┘
```

**Display Elements:**
- Large total contribution count
- Four statistic cards (longest streak, max in a day, current streak, average)
- 53×7 contribution heatmap with grayscale intensity
- Footer with GitHub info, WiFi signal strength, and current time

## Configuration

### Update Interval

The deep sleep duration is configured in `main.cpp` in the `goDeepSleep()` function:

```cpp
// Current: 6 hours (21,600,000,000 microseconds = 6 × 60 × 60 × 1,000,000)
esp_sleep_enable_timer_wakeup(2.16e10);
```

Modify this value to change the update frequency:

| Interval  | Microseconds | Scientific Notation |
|-----------|--------------|---------------------|
| 30 min    | 1,800,000,000| `1.8e9`            |
| 1 hour    | 3,600,000,000| `3.6e9`            |
| 3 hours   | 10,800,000,000| `1.08e10`         |
| 6 hours   | 21,600,000,000| `2.16e10`         |
| 12 hours  | 43,200,000,000| `4.32e10`         |
| 24 hours  | 86,400,000,000| `8.64e10`         |

> **Note**: More frequent updates will consume more power and may impact battery life if running on batteries.

### Timezone Configuration

Adjust the timezone in the `setup()` function in `main.cpp`:

```cpp
// Parameters: gmtOffset_sec, daylightOffset_sec, ntpServer
configTime(3600, 3600, "pool.ntp.org");
```

**Parameters:**
- `gmtOffset_sec`: Timezone offset from GMT in seconds
- `daylightOffset_sec`: Daylight Saving Time offset in seconds (usually 3600 or 0)
- `ntpServer`: NTP server address

**Common Timezone Examples:**

| Timezone           | GMT Offset | DST Offset | Example                        |
|--------------------|------------|------------|--------------------------------|
| GMT/UTC            | 0          | 0          | `configTime(0, 0, ...)`        |
| CET (GMT+1)        | 3600       | 3600       | `configTime(3600, 3600, ...)`  |
| EST (GMT-5)        | -18000     | 3600       | `configTime(-18000, 3600, ...)` |
| PST (GMT-8)        | -28800     | 3600       | `configTime(-28800, 3600, ...)` |
| JST (GMT+9)        | 32400      | 0          | `configTime(32400, 0, ...)`    |

> **Formula**: Offset in seconds = Hours × 3600

## Troubleshooting

### WiFi Connection Failed

**Symptoms**: Display shows "WiFi Connection failed" message

**Solutions**:
- ✓ Verify SSID and password in `include/resources/credentials.h`
- ✓ Ensure you're using **2.4GHz WiFi** (ESP32 doesn't support 5GHz)
- ✓ Check WiFi signal strength—move ESP32 closer to the router
- ✓ Try a different WiFi network to rule out router issues
- ✓ Monitor serial output for detailed error messages (`pio device monitor`)
- ✓ Verify your WiFi doesn't require captive portal login

### Display Not Updating or Shows Garbage

**Symptoms**: Blank screen, partial content, or corrupted graphics

**Solutions**:
- ✓ Double-check all pin connections (especially SPI pins)
- ✓ Verify the BSY (Busy) pin—it should go LOW when the display is ready
- ✓ Ensure stable 3.3V power supply (USB power from PC/adapter should work)
- ✓ Try a shorter refresh: the display needs adequate voltage during updates
- ✓ Check if your e-paper model matches the code (GDEY075T7)
- ✓ Test with a simple GxEPD2 example sketch first

### GitHub API Errors

**Symptoms**: HTTP errors, no data displayed, or JSON parsing failures

**Solutions**:
- ✓ Verify GitHub Personal Access Token has `read:user` scope
- ✓ Check if the token has expired—generate a new one if needed
- ✓ Confirm the username is correct (case-sensitive)
- ✓ Monitor serial output for HTTP status codes:
  - `401 Unauthorized`: Invalid or expired token
  - `403 Forbidden`: Rate limit exceeded or insufficient permissions
  - `404 Not Found`: Username doesn't exist
- ✓ Test the API manually: `curl -H "Authorization: Bearer YOUR_TOKEN" https://api.github.com/users/YOUR_USERNAME`

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

### Power Profile

| Mode                          | Current Draw | Duration      |
|-------------------------------|--------------|---------------|
| Active (WiFi + API calls)     | ~150-200mA   | ~20-30 sec    |
| Display update (full refresh) | ~50-100mA    | ~5-10 sec     |
| Deep sleep                    | ~10-15µA     | ~6 hours      |

### Battery Life Estimation

**Average current over 6-hour cycle**: ~0.2-0.5mA

**Battery Life Examples**:
- 1000mAh LiPo battery: ~3-6 months
- 2000mAh LiPo battery: ~6-12 months
- 5000mAh power bank: 1+ year

> **Note**: Actual battery life depends on update frequency, WiFi signal strength, API response times, and battery self-discharge rate.

### Power Optimization Tips

- Increase deep sleep duration (e.g., 12 or 24 hours)
- Use partial display refresh when possible (requires code modification)
- Reduce WiFi timeout from 30 seconds if your network is fast
- Consider adding a low-battery voltage check before updates

## Future Enhancements

Planned features and improvements:

- [ ] **User Interface**
  - [ ] Display follower/following counts
  - [ ] Show repository count and total stars
  - [ ] Add language breakdown chart
  - [ ] Customizable themes (dark mode, color schemes)
  
- [ ] **Power Management**
  - [ ] Battery voltage monitoring and low-battery warning
  - [ ] Adaptive update intervals based on battery level
  - [ ] Solar panel integration support
  
- [ ] **Configuration**
  - [ ] Web-based configuration portal (WiFi manager)
  - [ ] OTA (Over-The-Air) firmware updates
  - [ ] Multiple GitHub account profiles with switching
  
- [ ] **Display Optimization**
  - [ ] Partial refresh support for faster updates
  - [ ] Red/black/white display variant support
  - [ ] Multiple display size support

- [ ] **Features**
  - [ ] Show recent commits or activity
  - [ ] Integration with other services (GitLab, Bitbucket)
  - [ ] QR code linking to GitHub profile

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

This project wouldn't be possible without these excellent libraries and resources:

- **[GxEPD2](https://github.com/ZinggJM/GxEPD2)** by Jean-Marc Zingg - Comprehensive e-paper display library
- **[ArduinoJson](https://arduinojson.org/)** by Benoît Blanchon - Efficient and elegant JSON parsing
- **[GitHub GraphQL API](https://docs.github.com/en/graphql)** - For contribution calendar data
- **[Roboto Font Family](https://fonts.google.com/specimen/Roboto)** by Christian Robertson - Clean, modern typography
- **ESP32 Arduino Core** - Arduino framework implementation for ESP32

## Author

**ToniF03**
- GitHub: [@ToniF03](https://github.com/ToniF03)

## Support

If you find this project helpful, please give it a ⭐ on GitHub!

---

## Security Best Practices

⚠️ **Important Reminders**:

1. **Never commit credentials** - Keep your `credentials.h` file private
2. **Use `.gitignore`** - Add `include/resources/credentials.h` to prevent accidental commits
3. **Rotate tokens regularly** - Regenerate your GitHub PAT periodically
4. **Use minimal permissions** - Only grant the `read:user` scope, nothing more
5. **Create a template** - Consider creating `credentials.h.example` with placeholder values

**Example `.gitignore` entry**:
```
include/resources/credentials.h
.pio/
.vscode/
```
