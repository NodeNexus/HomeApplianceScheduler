#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <RTClib.h>

#define RELAY1 19
#define RELAY2 18

const char* ssid = "ESP32-AP";
const char* password = "12345678";

AsyncWebServer server(80);
RTC_DS3231 rtc;

struct Schedule {
  int hour;
  int minute;
  bool state; // ON or OFF
};

Schedule device1Schedule[7]; // One entry per day for simplicity
Schedule device2Schedule[7];

void setup() {
  Serial.begin(115200);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP());

  if(!rtc.begin()){
    Serial.println("RTC not found!");
    while(1);
  }

  // Default schedules
  for(int i=0;i<7;i++){
    device1Schedule[i] = {8, 0, true};
    device2Schedule[i] = {20, 0, false};
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", webpage);
  });

  server.on("/setSchedule", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("device") && request->hasParam("day") &&
       request->hasParam("hour") && request->hasParam("minute") && request->hasParam("state")){
      String dev = request->getParam("device")->value();
      int day = request->getParam("day")->value().toInt();
      int hour = request->getParam("hour")->value().toInt();
      int min = request->getParam("minute")->value().toInt();
      bool st = request->getParam("state")->value() == "1";
      if(dev=="1") device1Schedule[day] = {hour,min,st};
      else device2Schedule[day] = {hour,min,st};
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.begin();
}

void loop() {
  DateTime now = rtc.now();
  int weekday = now.dayOfTheWeek(); // 0=Sun, 6=Sat

  Schedule s1 = device1Schedule[weekday];
  Schedule s2 = device2Schedule[weekday];

  digitalWrite(RELAY1, (now.hour()==s1.hour && now.minute()==s1.minute && s1.state) ? HIGH : LOW);
  digitalWrite(RELAY2, (now.hour()==s2.hour && now.minute()==s2.minute && s2.state) ? HIGH : LOW);

  delay(1000);
}

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Home Appliance Scheduler</title>
<style>
body { font-family: Arial; background-color: #111; color: #0ff; text-align: center; }
h1 { margin-bottom: 20px; }
label { margin: 5px; }
input, select { margin: 5px; }
button { padding: 5px 10px; margin: 10px; }
</style>
</head>
<body>
<h1>Appliance Scheduler Dashboard</h1>
<div>
<label>Device:</label>
<select id="device"><option value="1">Device 1</option><option value="2">Device 2</option></select><br>
<label>Day (0=Sun):</label><input type="number" id="day" min="0" max="6"><br>
<label>Hour:</label><input type="number" id="hour" min="0" max="23"><br>
<label>Minute:</label><input type="number" id="minute" min="0" max="59"><br>
<label>State (1=ON,0=OFF):</label><input type="number" id="state" min="0" max="1"><br>
<button onclick="setSchedule()">Set Schedule</button>
<div id="status"></div>
</div>

<script>
function setSchedule(){
  let device = document.getElementById('device').value;
  let day = document.getElementById('day').value;
  let hour = document.getElementById('hour').value;
  let minute = document.getElementById('minute').value;
  let state = document.getElementById('state').value;

  fetch(`/setSchedule?device=${device}&day=${day}&hour=${hour}&minute=${minute}&state=${state}`)
  .then(resp => resp.text())
  .then(txt => { document.getElementById('status').innerText = txt; });
}
</script>
</body>
</html>
)rawliteral";
