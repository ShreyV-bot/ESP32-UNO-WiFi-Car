#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID = "ESP32_CAR";
const char* AP_PASS = "12345678";

WebServer server(80);

// ESP32 Serial2 pins
#define ESP32_RX2 16
#define ESP32_TX2 17

int currentSpeed = 180;

void sendCmd(const String& c) {
  Serial2.println(c);
}

String page() {
  String p = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Car</title>
  <style>
    body{
      background:#111;
      color:#fff;
      font-family:Arial;
      text-align:center;
      margin:0;
      padding:20px;
    }
    button{
      width:120px;
      height:60px;
      margin:8px;
      font-size:20px;
      border:none;
      border-radius:12px;
    }
    input[type=range]{
      width:320px;
      max-width:90vw;
    }
  </style>
</head>
<body>
  <h2>ESP32 Wi-Fi Car</h2>

  <div>
    <button onclick="cmd('F')">Forward</button>
  </div>

  <div>
    <button onclick="cmd('L')">Left</button>
    <button onclick="cmd('S')">Stop</button>
    <button onclick="cmd('R')">Right</button>
  </div>

  <div>
    <button onclick="cmd('B')">Back</button>
  </div>

  <p>Speed: <span id="sv">180</span></p>
  <input type="range" min="0" max="255" value="180" oninput="spd(this.value)">

  <script>
    function cmd(c){
      fetch('/cmd?c=' + c).catch(()=>{});
    }
    function spd(v){
      document.getElementById('sv').innerText = v;
      fetch('/speed?v=' + v).catch(()=>{});
    }
  </script>
</body>
</html>
)rawliteral";
  return p;
}

void handleRoot() {
  server.send(200, "text/html", page());
}

void handleCmd() {
  if (server.hasArg("c")) {
    String c = server.arg("c");
    sendCmd(c);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing cmd");
  }
}

void handleSpeed() {
  if (server.hasArg("v")) {
    currentSpeed = constrain(server.arg("v").toInt(), 0, 255);
    sendCmd("V" + String(currentSpeed));
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing speed");
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, ESP32_RX2, ESP32_TX2);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/speed", handleSpeed);
  server.begin();

  Serial.println("ESP32 AP started");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
