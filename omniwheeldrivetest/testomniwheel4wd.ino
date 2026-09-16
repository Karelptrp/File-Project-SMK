#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "nganuanbot";
const char *password = "1sampai8";

AsyncWebServer server(80);
AsyncWebSocket ws("/CarInput");

// motor dc depan kiri
const int FL_IN1 = 13;
const int FL_IN2 = 12;
const int FL_EN  = 23;

//motor dc depan kanan
const int FR_IN1 = 14;
const int FR_IN2 = 27;
const int FR_EN  = 22;

// motor dc belakang kiri
const int BL_IN1 = 26;
const int BL_IN2 = 25;
const int BL_EN  = 21;

// motor dc belakang kanan
const int BR_IN1 = 33;
const int BR_IN2 = 32;
const int BR_EN  = 19;


const bool INVERT_FL = false;
const bool INVERT_FR = false;
const bool INVERT_BL = false;
const bool INVERT_BR = false;

// Default speed motornya
int motorSpeed = 170;

// Movement commands
enum CarMovement {
  STOP = 0,
  FORWARD = 1,
  BACKWARD = 2,
  LEFT = 3,
  RIGHT = 4,
  FORWARD_LEFT = 5,
  FORWARD_RIGHT = 6,
  BACKWARD_LEFT = 7,
  BACKWARD_RIGHT = 8,
  ROTATE_LEFT = 9,
  ROTATE_RIGHT = 10
};


const char htmlHomePage[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1,
    maximum-scale=1, user-scalable=no"
  >

  <title>test omnidir bot</title>

  <style>
    * {
      box-sizing: border-box;
      -webkit-tap-highlight-color: transparent;
    }

    body {
      margin: 0;
      min-height: 100vh;
      background:
        radial-gradient(circle at top, #17253d 0%, #08101d 55%, #03060c 100%);
      color: white;
      font-family: Arial, Helvetica, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      user-select: none;
      touch-action: none;
    }

    .dashboard {
      width: min(94vw, 470px);
      background: rgba(13, 25, 43, 0.96);
      border: 1px solid #294466;
      border-radius: 24px;
      padding: 22px;
      box-shadow: 0 20px 55px rgba(0, 0, 0, 0.55);
    }

    .brand {
      text-align: center;
      font-size: 12px;
      letter-spacing: 4px;
      color: #38bdf8;
      margin-bottom: 5px;
    }

    h1 {
      text-align: center;
      font-size: 25px;
      margin: 0;
    }

    .subtitle {
      text-align: center;
      color: #94a3b8;
      font-size: 13px;
      margin-top: 7px;
      margin-bottom: 18px;
    }

    .status-box {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      background: #0a1423;
      border-radius: 12px;
      padding: 10px;
      margin-bottom: 18px;
      font-size: 14px;
    }

    .status-dot {
      width: 11px;
      height: 11px;
      border-radius: 50%;
      background: #ef4444;
      box-shadow: 0 0 10px #ef4444;
    }

    .status-dot.connected {
      background: #22c55e;
      box-shadow: 0 0 10px #22c55e;
    }

    .controls {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 11px;
    }

    .control-button {
      height: 72px;
      border: none;
      border-radius: 17px;
      color: white;
      background: linear-gradient(145deg, #1e3553, #10223a);
      border: 1px solid #315273;
      box-shadow:
        0 6px 0 #060c15,
        inset 0 1px 0 rgba(255, 255, 255, 0.08);
      font-size: 29px;
      font-weight: bold;
      cursor: pointer;
      touch-action: none;
    }

    .control-button:active,
    .control-button.active {
      transform: translateY(5px);
      box-shadow:
        0 1px 0 #060c15,
        inset 0 0 15px rgba(56, 189, 248, 0.35);
      background: #0369a1;
    }

    .stop-button {
      background: linear-gradient(145deg, #dc2626, #8f1717);
      border-color: #ef4444;
      font-size: 17px;
      letter-spacing: 1px;
    }

    .rotate-button {
      color: #38bdf8;
      font-size: 33px;
    }

    .speed-section {
      margin-top: 24px;
      background: #0a1423;
      border-radius: 16px;
      padding: 17px;
    }

    .speed-heading {
      display: flex;
      justify-content: space-between;
      margin-bottom: 13px;
      font-size: 15px;
    }

    #speedValue {
      color: #38bdf8;
      font-weight: bold;
    }

    input[type="range"] {
      width: 100%;
      cursor: pointer;
      accent-color: #0ea5e9;
    }

    .instruction {
      text-align: center;
      color: #718096;
      font-size: 12px;
      line-height: 1.5;
      margin-top: 16px;
    }
  </style>
</head>

<body>

  <div class="dashboard">

    <div class="brand">TEST OMNIDIR BOT</div>

    <h1>Omni-Wheel Car</h1>

    <div class="subtitle">
      ESP32 direct Wi-Fi control dashboard
    </div>

    <div class="status-box">
      <div id="statusDot" class="status-dot"></div>
      <span id="statusText">Connecting...</span>
    </div>

    <div class="controls">

      <button
        class="control-button"
        data-command="5"
      >
        ↖
      </button>

      <button
        class="control-button"
        data-command="1"
      >
        ↑
      </button>

      <button
        class="control-button"
        data-command="6"
      >
        ↗
      </button>

      <button
        class="control-button"
        data-command="3"
      >
        ←
      </button>

      <button
        class="control-button stop-button"
        data-command="0"
      >
        STOP
      </button>

      <button
        class="control-button"
        data-command="4"
      >
        →
      </button>

      <button
        class="control-button"
        data-command="7"
      >
        ↙
      </button>

      <button
        class="control-button"
        data-command="2"
      >
        ↓
      </button>

      <button
        class="control-button"
        data-command="8"
      >
        ↘
      </button>

      <button
        class="control-button rotate-button"
        data-command="9"
      >
        ↶
      </button>

      <div></div>

      <button
        class="control-button rotate-button"
        data-command="10"
      >
        ↷
      </button>

    </div>

    <div class="speed-section">

      <div class="speed-heading">
        <span>Motor speed</span>
        <span id="speedValue">170</span>
      </div>

      <input
        type="range"
        id="speedSlider"
        min="70"
        max="255"
        value="170"
      >

    </div>

    <div class="instruction">
      Press and hold a direction button to move.<br>
      Release the button to stop the car.
    </div>

  </div>

<script>
  let websocket;
  let reconnectTimer;

  const statusDot = document.getElementById("statusDot");
  const statusText = document.getElementById("statusText");
  const speedSlider = document.getElementById("speedSlider");
  const speedValue = document.getElementById("speedValue");

  function connectWebSocket() {
    websocket = new WebSocket(
      "ws://" + window.location.hostname + "/CarInput"
    );

    websocket.onopen = function() {
      statusDot.classList.add("connected");
      statusText.textContent = "ESP32 connected";

      sendCommand("Speed", speedSlider.value);
      sendCommand("MoveCar", 0);
    };

    websocket.onclose = function() {
      statusDot.classList.remove("connected");
      statusText.textContent = "Disconnected";

      clearTimeout(reconnectTimer);
      reconnectTimer = setTimeout(connectWebSocket, 1500);
    };

    websocket.onerror = function() {
      statusText.textContent = "Connection error";
    };
  }

  function sendCommand(key, value) {
    if (
      websocket &&
      websocket.readyState === WebSocket.OPEN
    ) {
      websocket.send(key + "," + value);
    }
  }

  function startMovement(button) {
    const command = button.dataset.command;

    document.querySelectorAll(".control-button").forEach(btn => {
      btn.classList.remove("active");
    });

    button.classList.add("active");
    sendCommand("MoveCar", command);
  }

  function stopMovement(button) {
    button.classList.remove("active");
    sendCommand("MoveCar", 0);
  }

  document.querySelectorAll(".control-button").forEach(button => {

    button.addEventListener("pointerdown", function(event) {
      event.preventDefault();
      button.setPointerCapture(event.pointerId);
      startMovement(button);
    });

    button.addEventListener("pointerup", function(event) {
      event.preventDefault();
      stopMovement(button);
    });

    button.addEventListener("pointercancel", function() {
      stopMovement(button);
    });

    button.addEventListener("pointerleave", function(event) {
      if (event.buttons === 1) {
        stopMovement(button);
      }
    });
  });

  speedSlider.addEventListener("input", function() {
    speedValue.textContent = speedSlider.value;
    sendCommand("Speed", speedSlider.value);
  });

  window.addEventListener("blur", function() {
    sendCommand("MoveCar", 0);
  });

  document.addEventListener("visibilitychange", function() {
    if (document.hidden) {
      sendCommand("MoveCar", 0);
    }
  });

  window.onload = connectWebSocket;
</script>

</body>
</html>
)HTML";

// Individual motor control
//
// motorValue:
//  255  = max forward
//  0    = stop
// -255  = max backward

void controlSingleMotor(
  int in1,
  int in2,
  int enablePin,
  int motorValue,
  bool inverted
) {
  motorValue = constrain(motorValue, -255, 255);

  if (inverted) {
    motorValue = -motorValue;
  }

  int pwmValue = abs(motorValue);

  if (motorValue > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enablePin, pwmValue);
  }
  else if (motorValue < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enablePin, pwmValue);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enablePin, 0);
  }
}


// kontrol 4 motor


void driveMotors(
  int frontLeft,
  int frontRight,
  int backLeft,
  int backRight
) {
  controlSingleMotor(
    FL_IN1, FL_IN2, FL_EN,
    frontLeft, INVERT_FL
  );

  controlSingleMotor(
    FR_IN1, FR_IN2, FR_EN,
    frontRight, INVERT_FR
  );

  controlSingleMotor(
    BL_IN1, BL_IN2, BL_EN,
    backLeft, INVERT_BL
  );

  controlSingleMotor(
    BR_IN1, BR_IN2, BR_EN,
    backRight, INVERT_BR
  );
}


// Omni-wheel movementnya

void moveCar(int movement) {
  int speedValue = motorSpeed;

  switch (movement) {

    case FORWARD:
      driveMotors(
        speedValue,
        speedValue,
        speedValue,
        speedValue
      );
      break;

    case BACKWARD:
      driveMotors(
        -speedValue,
        -speedValue,
        -speedValue,
        -speedValue
      );
      break;

case LEFT:
  driveMotors(
     speedValue,
    -speedValue,
    -speedValue,
     speedValue
  );
  break;

case RIGHT:
  driveMotors(
    -speedValue,
     speedValue,
     speedValue,
    -speedValue
  );
  break;

case FORWARD_LEFT:
  driveMotors(
    speedValue,
    0,
    0,
    speedValue
  );
  break;

case FORWARD_RIGHT:
  driveMotors(
    0,
    speedValue,
    speedValue,
    0
  );
  break;

case BACKWARD_LEFT:
  driveMotors(
    0,
    -speedValue,
    -speedValue,
    0
  );
  break;

case BACKWARD_RIGHT:
  driveMotors(
    -speedValue,
    0,
    0,
    -speedValue
  );
  break;

    case ROTATE_LEFT:
      driveMotors(
        -speedValue,
         speedValue,
        -speedValue,
         speedValue
      );
      break;

    case ROTATE_RIGHT:
      driveMotors(
         speedValue,
        -speedValue,
         speedValue,
        -speedValue
      );
      break;

    case STOP:
    default:
      driveMotors(0, 0, 0, 0);
      break;
  }
}

// Process WebSocket command

void processCarCommand(String message) {
  int commaPosition = message.indexOf(',');

  if (commaPosition == -1) {
    return;
  }

  String key = message.substring(0, commaPosition);
  String value = message.substring(commaPosition + 1);

  int valueInt = value.toInt();

  if (key == "MoveCar") {
    moveCar(valueInt);

    Serial.print("Movement command: ");
    Serial.println(valueInt);
  }

  else if (key == "Speed") {
    motorSpeed = constrain(valueInt, 70, 255);

    Serial.print("Motor speed: ");
    Serial.println(motorSpeed);
  }
}

// WebSocket event

void onWebSocketEvent(
  AsyncWebSocket *server,
  AsyncWebSocketClient *client,
  AwsEventType type,
  void *arg,
  uint8_t *data,
  size_t len
) {
  if (type == WS_EVT_CONNECT) {
    Serial.print("Dashboard connected. Client ID: ");
    Serial.println(client->id());

    moveCar(STOP);
  }

  else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Dashboard disconnected");

    // Safety stop
    moveCar(STOP);
  }

  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (
      info->final &&
      info->index == 0 &&
      info->len == len &&
      info->opcode == WS_TEXT
    ) {
      String message = "";

      for (size_t i = 0; i < len; i++) {
        message += (char)data[i];
      }

      processCarCommand(message);
    }
  }
}

// Motor pin setup

void setupMotorPins() {
  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);
  pinMode(FL_EN, OUTPUT);

  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);
  pinMode(FR_EN, OUTPUT);

  pinMode(BL_IN1, OUTPUT);
  pinMode(BL_IN2, OUTPUT);
  pinMode(BL_EN, OUTPUT);

  pinMode(BR_IN1, OUTPUT);
  pinMode(BR_IN2, OUTPUT);
  pinMode(BR_EN, OUTPUT);

  moveCar(STOP);
}

// Setup

void setup() {
  Serial.begin(115200);

  setupMotorPins();

  WiFi.mode(WIFI_AP);

  bool apStarted = WiFi.softAP(ssid, password);

  if (apStarted) {
    Serial.println();
    Serial.println("=================================");
    Serial.println("TEST OMNIDIR BOT");
    Serial.println("=================================");
    Serial.print("Wi-Fi name: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("Dashboard IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("=================================");
  }
  else {
    Serial.println("Failed to start Wi-Fi Access Point");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(
      200,
      "text/html",
      htmlHomePage
    );
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(
      404,
      "text/plain",
      "Page not found"
    );
  });

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  server.begin();

  Serial.println("Web control server started");
}

// Loop

void loop() {
  ws.cleanupClients();
}