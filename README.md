# ESP8266 Interactive Board

An interactive web-based drawing board built using an **ESP8266** and a **128×64 SSD1306 OLED display**.

The ESP8266 creates its own WiFi network and hosts a web interface. A phone or computer can connect to the ESP8266, open the webpage, and draw on the screen. The drawing is then sent to the ESP8266 and displayed on the OLED.

## Features

- ESP8266 acts as a standalone WiFi access point
- No internet connection required
- Web-based drawing interface
- Supports touchscreen input
- Supports mouse input
- 128×64 drawing canvas
- Drawing mode
- Eraser mode
- Adjustable brush size
- Clear button
- SSD1306 OLED display output

## Hardware

| Component | Quantity |
|---|---:|
| ESP8266 | 1 |
| SSD1306 OLED 128×64 | 1 |
| Jumper wires | As required |
| USB cable | 1 |

## OLED Connections

The OLED uses I²C communication.

| OLED | ESP8266 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | D6 |
| SCL | D5 |

### Pin Configuration

```cpp
#define SDA_PIN D6
#define SCL_PIN D5
```

The I²C interface is initialized using:

```cpp
Wire.begin(SDA_PIN, SCL_PIN);
```

## How It Works

```text
        Phone / Computer
               |
               | WiFi
               v
       +----------------+
       |    ESP8266     |
       |                |
       |   Web Server   |
       +-------+--------+
               |
               | I²C
               v
       +----------------+
       |  SSD1306 OLED  |
       |    128 x 64    |
       +----------------+
```

The ESP8266 creates its own WiFi access point and hosts the drawing webpage.

The user connects to the ESP8266 using a phone or computer and opens the web interface.

The webpage contains a 128×64 drawing canvas that matches the resolution of the OLED.

When something is drawn, the browser converts the canvas into monochrome pixel data and sends it to the ESP8266.

The ESP8266 processes the received data and displays the drawing on the OLED.

## Getting Started

### 1. Install the Required Libraries

Install the following libraries using the Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306

The project also uses the ESP8266 WiFi and web server libraries.

### 2. Upload the Code

Open the Arduino sketch:

```text
ESP8266_Interactive_Board.ino
```

Select the appropriate ESP8266 board and COM port in Arduino IDE.

Upload the code to the ESP8266.

### 3. Connect to the ESP8266

After powering the ESP8266, connect your phone or computer to the WiFi network:

```text
SSID: ESP8266-Board
Password: 12345678
```

### 4. Open the Web Interface

Open a browser and enter:

```text
http://192.168.4.1
```

The interactive drawing board will appear.

### 5. Start Drawing

Use your finger on a touchscreen or a mouse on a computer to draw.

The drawing will be displayed on the OLED.

## System Flow

```text
ESP8266 starts
      |
      v
Creates WiFi Access Point
      |
      v
User connects phone/computer
      |
      v
Opens 192.168.4.1
      |
      v
ESP8266 serves drawing webpage
      |
      v
User draws on canvas
      |
      v
Browser converts drawing to pixel data
      |
      v
Data sent to ESP8266
      |
      v
ESP8266 updates OLED
```

## WiFi Configuration

The ESP8266 creates the following network:

```text
Network: ESP8266-Board
Password: 12345678
IP Address: 192.168.4.1
```

These settings can be changed in the Arduino code.

## Project Structure

```text
ESP8266-Interactive-Board/
│
├── ESP8266_Interactive_Board.ino
├── README.md
├── LICENSE
└── .gitignore
```

## Current Status

**Working Prototype**

The current version includes:

- WiFi access point
- Web server
- Drawing webpage
- Touchscreen drawing
- Mouse drawing
- Eraser
- Brush size control
- Clear function
- OLED display output
- Web-to-ESP8266 communication

## Limitations

The current implementation sends the complete 128×64 bitmap when the drawing is updated.

```text
128 × 64 = 8192 pixels
```

This works for the current prototype, but transmitting the complete bitmap is not the most efficient approach for smooth real-time drawing.

## Future Improvements

- Real-time drawing updates
- Send only changed pixels
- Reduce network traffic
- Smoother drawing
- Improved touch handling
- Multiple brush types
- Better eraser functionality
- Undo and redo
- Save drawings
- Load saved drawings
- Lines, rectangles, and circles
- Multiple drawing pages
- Improved mobile interface
- More efficient data transmission

## Technologies Used

- ESP8266
- Arduino IDE
- C++
- HTML
- CSS
- JavaScript
- WiFi
- HTTP
- I²C
- SSD1306 OLED

## Libraries Used

```text
ESP8266WiFi
ESP8266WebServer
Wire
Adafruit GFX Library
Adafruit SSD1306
```

## Demo

<img width="2304" height="4096" alt="IMG20260926174221" src="https://github.com/user-attachments/assets/0e9a1780-362e-41d7-9ea6-5263b6dde8aa" />
<img width="720" height="1280" alt="IMG_20260926_17450740" src="https://github.com/user-attachments/assets/1a287b42-6646-414c-8972-22e3ab6ccd2e" />
<img width="720" height="1280" alt="IMG_20260926_17451815" src="https://github.com/user-attachments/assets/64eeab78-782c-42ef-bf86-38ff1e58db92" />
<img width="720" height="1280" alt="IMG_20260926_17452143" src="https://github.com/user-attachments/assets/b5808287-ba2b-4688-a2e5-7257377e4bf8" />
<img width="720" height="1280" alt="IMG_20260926_17452859" src="https://github.com/user-attachments/assets/3eeb860c-b1e2-4cb6-9fb8-a82df414f398" />

```markdown
![ESP8266 Interactive Board](media/interactive-board.jpg)
```

## 🧑‍💻 Author

**Sahil B Pillai**  
Engineer | Robotics & AI Enthusiast  

---

## 📄 License

This project is open-source and available under the MIT License.
