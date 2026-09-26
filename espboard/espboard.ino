#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =========================
// OLED
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN D6
#define SCL_PIN D5

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =========================
// WiFi Access Point
// =========================
const char* ssid = "ESP8266-Board";
const char* password = "12345678";

ESP8266WebServer server(80);

// =========================
// HTML WEBPAGE
// =========================
const char webpage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport"
      content="width=device-width, initial-scale=1.0, user-scalable=no">

<title>ESP8266 Interactive Board</title>

<style>

* {
    box-sizing: border-box;
}

body {
    margin: 0;
    background: #222;
    color: white;
    font-family: Arial, sans-serif;
    text-align: center;
    touch-action: none;
}

h2 {
    margin: 12px 0;
}

#canvas {
    background: black;
    border: 2px solid white;

    width: min(96vw, 768px);
    height: auto;

    image-rendering: pixelated;
    touch-action: none;
}

.controls {
    margin-top: 12px;
}

button {
    padding: 10px 18px;
    margin: 4px;

    font-size: 16px;
    border: none;
    border-radius: 6px;
}

#clear {
    background: #e53935;
    color: white;
}

#erase {
    background: #777;
    color: white;
}

input {
    vertical-align: middle;
}

</style>

</head>

<body>

<h2>ESP8266 Interactive Board</h2>

<canvas id="canvas"
        width="128"
        height="64">
</canvas>

<div class="controls">

    <button id="clear">Clear</button>

    <button id="erase">Eraser</button>

    <br><br>

    Brush:
    <input
        type="range"
        id="size"
        min="1"
        max="5"
        value="1"
    >

</div>

<script>

const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

ctx.fillStyle = "black";
ctx.fillRect(0, 0, 128, 64);

ctx.strokeStyle = "white";
ctx.lineWidth = 1;
ctx.lineCap = "round";
ctx.lineJoin = "round";

let drawing = false;
let erasing = false;

function getPosition(e)
{
    const rect = canvas.getBoundingClientRect();

    let clientX;
    let clientY;

    if (e.touches && e.touches.length > 0)
    {
        clientX = e.touches[0].clientX;
        clientY = e.touches[0].clientY;
    }
    else
    {
        clientX = e.clientX;
        clientY = e.clientY;
    }

    let x = (clientX - rect.left)
          * canvas.width / rect.width;

    let y = (clientY - rect.top)
          * canvas.height / rect.height;

    x = Math.max(0, Math.min(127, x));
    y = Math.max(0, Math.min(63, y));

    return {
        x: x,
        y: y
    };
}

function startDrawing(e)
{
    e.preventDefault();

    drawing = true;

    const p = getPosition(e);

    ctx.beginPath();
    ctx.moveTo(p.x, p.y);

    // Draw a dot
    ctx.fillStyle = erasing ? "black" : "white";

    ctx.beginPath();
    ctx.arc(
        p.x,
        p.y,
        ctx.lineWidth / 2,
        0,
        Math.PI * 2
    );

    ctx.fill();
}

function draw(e)
{
    if (!drawing)
        return;

    e.preventDefault();

    const p = getPosition(e);

    ctx.strokeStyle = erasing ? "black" : "white";

    ctx.lineWidth =
        document.getElementById("size").value;

    ctx.lineTo(p.x, p.y);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(p.x, p.y);
}

function stopDrawing(e)
{
    if (e)
        e.preventDefault();

    drawing = false;

    ctx.beginPath();
}

canvas.addEventListener(
    "mousedown",
    startDrawing
);

canvas.addEventListener(
    "mousemove",
    draw
);

canvas.addEventListener(
    "mouseup",
    stopDrawing
);

canvas.addEventListener(
    "mouseleave",
    stopDrawing
);

canvas.addEventListener(
    "touchstart",
    startDrawing,
    { passive: false }
);

canvas.addEventListener(
    "touchmove",
    draw,
    { passive: false }
);

canvas.addEventListener(
    "touchend",
    stopDrawing,
    { passive: false }
);


// =========================
// CLEAR
// =========================

document.getElementById("clear").onclick = function()
{
    ctx.fillStyle = "black";

    ctx.fillRect(
        0,
        0,
        canvas.width,
        canvas.height
    );

    sendToOLED();
};


// =========================
// ERASER
// =========================

document.getElementById("erase").onclick = function()
{
    erasing = !erasing;

    this.innerText =
        erasing ? "Pen" : "Eraser";
};


// =========================
// AUTOMATIC OLED UPDATE
// =========================

let sendTimer;

canvas.addEventListener("touchend", function()
{
    clearTimeout(sendTimer);

    sendTimer = setTimeout(
        sendToOLED,
        100
    );
});

canvas.addEventListener("mouseup", function()
{
    clearTimeout(sendTimer);

    sendTimer = setTimeout(
        sendToOLED,
        100
    );
});


// =========================
// SEND DRAWING TO OLED
// =========================

function sendToOLED()
{
    const imageData =
        ctx.getImageData(
            0,
            0,
            128,
            64
        );

    let data = "";

    for (let y = 0; y < 64; y++)
    {
        for (let x = 0; x < 128; x++)
        {
            const index =
                (y * 128 + x) * 4;

            const brightness =
                imageData.data[index];

            data +=
                brightness > 128 ? "1" : "0";
        }
    }

    fetch("/draw", {
        method: "POST",
        headers: {
            "Content-Type":
                "text/plain"
        },
        body: data
    });
}

</script>

</body>

</html>

)rawliteral";


// =========================
// DRAW DATA RECEIVED
// =========================

void handleDraw()
{
    String data = server.arg("plain");

    if (data.length() != 8192)
    {
        server.send(
            400,
            "text/plain",
            "Invalid drawing data"
        );

        return;
    }

    display.clearDisplay();

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            int index =
                y * 128 + x;

            if (data[index] == '1')
            {
                display.drawPixel(
                    x,
                    y,
                    SSD1306_WHITE
                );
            }
        }
    }

    display.display();

    server.send(
        200,
        "text/plain",
        "OK"
    );
}


// =========================
// SETUP
// =========================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    // -------------------------
    // OLED
    // -------------------------

    Wire.begin(
        SDA_PIN,
        SCL_PIN
    );

    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            0x3C))
    {
        Serial.println(
            "OLED not found!"
        );

        while (true)
        {
            delay(1000);
        }
    }

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(1);

    display.setCursor(0, 0);

    display.println(
        "Interactive Board"
    );

    display.println();

    display.println(
        "Starting WiFi..."
    );

    display.display();


    // -------------------------
    // WiFi Access Point
    // -------------------------

    WiFi.mode(WIFI_AP);

    WiFi.softAP(
        ssid,
        password
    );

    IPAddress IP =
        WiFi.softAPIP();


    // -------------------------
    // OLED information
    // -------------------------

    display.clearDisplay();

    display.setCursor(0, 0);

    display.println(
        "Interactive Board"
    );

    display.println();

    display.println(
        "WiFi:"
    );

    display.println(
        ssid
    );

    display.println();

    display.println(
        "IP:"
    );

    display.println(
        IP
    );

    display.display();


    // -------------------------
    // Web server
    // -------------------------

    server.on(
        "/",
        HTTP_GET,
        []()
        {
            server.send_P(
                200,
                "text/html",
                webpage
            );
        }
    );

    server.on(
        "/draw",
        HTTP_POST,
        handleDraw
    );

    server.begin();

    Serial.println();
    Serial.println(
        "Interactive Board Started"
    );

    Serial.print(
        "WiFi: "
    );

    Serial.println(
        ssid
    );

    Serial.print(
        "IP: "
    );

    Serial.println(
        IP
    );
}


// =========================
// LOOP
// =========================

void loop()
{
    server.handleClient();
}
