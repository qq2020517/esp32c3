#include <WiFi.h>
#include <ESPmDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <PWMOutESP32.h>
#include <Button.h>
#include <Adafruit_NeoPixel.h>
Button BUT_1(18, B_PULLUP, 1000);
//Button BUT_2(19, B_PULLUP, 2000);
short but1 = 0;
#define LED_2 12
#define LED_1 13
#define LEDTYPE WS2812
#define LED_RGB 2
#define LED_NUM 20
#define LEDCOLOR GRB
const char* ssid = "WIFI_2.4";
const char* pswd = "a147z147";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.windows.com", 8 * 3600, 60000);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_RGB, NEO_GRB + NEO_KHZ800);
int Led[12][3] = {
  {242, 149, 0}, {226, 197, 0}, {164, 179, 0},
  {0, 154, 85}, {0, 107, 147}, {0, 86, 156}, 
  {171, 38, 100}
};
int flag = 0, count = 0, fadeLed = 1;
uint8_t LedR = 255, LedG = 255, LedB = 255;
int Pwm_Value = 1000 - map(Pwm_Value, 0, 1000, 0, 100);
int Bright = 75 - map(Bright, 0, 1023, 0, 255);
WebServer server(80);
PWMOutESP32 PWM(10, 2000);
const int LEDSHOW = 1 * 60;
long ledRunTime = 0;

void setup () {
  Serial.begin(115200);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_RGB, OUTPUT);
  strip.begin();
  strip.fill(strip.Color(0, 0, 0));
  strip.setBrightness(Bright);
  strip.show();

  connectWiFi();
  timeClient.begin();
  server.begin();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/Strong", HTTP_POST, strong);
  server.on("/Lighter", HTTP_POST, lighter);
  server.on("/Ledson", HTTP_POST, ledson);
  server.on("/Ledsoff", HTTP_POST, ledsoff);
  server.onNotFound(handleNotFound);

  printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
  timeClient.update();
  getTimer();
  printf("     ALL Dome ...... \n");
  printf("系统时间: %s \n", timeClient.getFormattedTime());
}
void loop () {
  if (millis() - ledRunTime > (1000L * LEDSHOW)) {
    getTimer();
    count++;
    if (count == 5) count = 0;
    ledRunTime = millis();
    printf("系统时间: %s \n", timeClient.getFormattedTime());
    Serial.printf("程序: %u \n", count);
  }
  if (count == 0) {
    PLAY_1();
  }
  if (count == 1) {
    PLAY_2();
  }
  if (count == 2) {
    PLAY_3();
  }
  if (count == 3) {
    PLAY_4();
  }
  if (count == 4) {
    PLAY_5();
  }
  server.handleClient();
}
/*********************************/
void handleRoot () {
  server.send(200, "text/html", sendHTML(Pwm_Value));
}
String sendHTML (int16_t Pwm_Value) {
  String htmlCode = "<!DOCTYPE html> <html>\n";
  htmlCode += "<head>\n";
  htmlCode += "<meta charset='utf-8'>\n";
  htmlCode += "<meta http-equiv='refresh' content='5'/>\n";
  htmlCode += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  htmlCode += "<title>Web控制页面</title>\n";
  htmlCode += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  htmlCode += "body{margin-top: 50px;} h1 {color: #ff0080;margin: 50px auto 30px;} h3 {color: #585858;margin-bottom: 50px;}\n";
  htmlCode += "h1 {color: #444444;margin: 50px auto 30px;}\n";
  htmlCode += "h2 {color: #444444;margin-bottom: 20px;}\n";
  htmlCode += "h3 {color: #585858;margin-bottom: 50px;}\n";
  htmlCode += ".button {display: block;width: 120px;background-color: #1abc9c;border: none;color: black;padding: 10px 0px;text-decoration: none;font-size: 15px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  htmlCode += ".button_on {background-color: #ff00ff;}\n";
  htmlCode += ".button_on:active {background-color: #DE341E;}\n";
  htmlCode += ".button_off {background-color: #5AD2FF;}\n";
  htmlCode += ".button_off:active {background-color: #DE341E;}\n";
  htmlCode += ".led_on {background-color: #00ff00;}\n"; //#dc143C #585858 #008000
  htmlCode += ".led_on: active {background-color: #DE341E;}\n";
  htmlCode += ".led_off {background-color: #ff0080;}\n";
  htmlCode += ".led_off: active {background-color: #DE341E;}\n";
  htmlCode += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  htmlCode += "</style>\n";
  htmlCode += "</head>\n";
  htmlCode += "<body>\n";
  htmlCode += "<h1>LED 控制页面</h1>\n";
  htmlCode += "<form action=\"/Ledsoff\" method=\"POST\"><input class=\"button button_on\" type=\"submit\" value=\"开  灯\"></form>\n";
  htmlCode += "<form action=\"/Ledson\" method=\"POST\"><input class=\"button button_off\" type=\"submit\" value=\"关  灯\"></form>\n";
  htmlCode += "<form action=\"/Strong\" method=\"POST\"><input class=\"button led_on\" type=\"submit\" value=\"亮度增加\"></form>\n";
  htmlCode += "<form action=\"/Lighter\" method=\"POST\"><input class=\"button led_off\" type=\"submit\" value=\"亮度减小\"></form>\n";
  htmlCode += "<h2>当前灯亮度数值: " + String((Pwm_Value) / 10) + " %</h2>\n";
  htmlCode += "</body>\n";
  htmlCode += "</html>\n";
  htmlCode += "\r\n";
  return htmlCode;
}
void handleNotFound () {
  server.send(404, "text/plain", "404: Not found");
}
void ledson () {
  Pwm_Value = 0;
  PWM.analogWrite(LED_1, Pwm_Value);
  PWM.analogWrite(LED_2, Pwm_Value);
  Serial.printf("  关 \r\n");
  server.sendHeader("Location", "/");
  server.send(303);
}
void ledsoff () {
  Pwm_Value = 1000;
  PWM.analogWrite(LED_1, Pwm_Value);
  PWM.analogWrite(LED_2, Pwm_Value);
  Serial.printf("  开 \r\n");
  server.sendHeader("Location", "/");
  server.send(303);
}
void strong () {
  Pwm_Value += 100;
  Pwm_Value >= 1000 ? 1000 : Pwm_Value;
  PWM.analogWrite(LED_1, Pwm_Value);
  PWM.analogWrite(LED_2, Pwm_Value);
  Serial.println(Pwm_Value);
  server.sendHeader("Location", "/");
  server.send(303);
}
void lighter () {
  Pwm_Value -= 100;
  Pwm_Value <= 0 ? 0 : Pwm_Value;
  PWM.analogWrite(LED_1, Pwm_Value);
  PWM.analogWrite(LED_2, Pwm_Value);
  Serial.println(Pwm_Value);
  server.sendHeader("Location", "/");
  server.send(303);
}
/*********************************/
void Set_RGB(int R, int G, int B, int val) {
  for (uint16_t i = 0; i < LED_NUM; i++) {
    strip.setPixelColor(i, R, G, B);
    strip.setBrightness(val);
  }
  strip.show();
}

void PLAY_1() {
  //Serial.printf("PLAY_1 \r\n");
  for (int j = 0; j < 7; j++) {
    for (int i = 0; i < LED_NUM; i++) {
      strip.setPixelColor(i, Led[j][0], Led[j][1], Led[j][2]);
      strip.setBrightness(Bright);
      strip.show();
      delay(60);
    }
    delay(500);
  }
}
void PLAY_2() {
  //Serial.printf("PLAY_2 \r\n");
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < LED_NUM; i++) {
      strip.setPixelColor(i, strip.Wheel((i * 256 / LED_NUM + j)));
      strip.setBrightness(Bright);
    }
    strip.show();
    delay(10);
  }
}
void PLAY_3() {
  //Serial.printf("PLAY_3 \r\n");
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < LED_NUM; c += 3) {
        uint32_t color = strip.gamma32(strip.ColorHSV(c * 65536L / LED_NUM));
        strip.setPixelColor(c, color);
        strip.setBrightness(Bright);
      }
      strip.show();
      delay(70);
    }
  }
}
void PLAY_4() {
  //Set_RGB(LedR, LedG, LedB, Bright);
  //int firstPixelHue = 0;
  for (int a = 0; a < 7; a++) {
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < LED_NUM; c += 3) {
        //int hue = firstPixelHue + c * 65536L / LED_NUM;
        //uint32_t color = strip.gamma32(strip.ColorHSV(hue));
        strip.setPixelColor(c, strip.Color(Led[a][0], Led[a][1], Led[a][2]));
        strip.setBrightness(Bright);
        delay(10);
      }
      strip.show();
      delay(10);
      //firstPixelHue += 65536 / 90;
    }
  }
}
void PLAY_5() {
  //Serial.printf("PLAY_5 \r\n");
  Set_RGB(LedR, LedG, LedB, Bright);
  Bright = Bright + fadeLed;
  if (Bright == 0 || Bright == 120) {
    fadeLed = - fadeLed;
    flag++;
  }
  if (flag == 2) {
    LedR = random(255);
    LedG = random(255);
    LedB = random(255);
    flag = 0;
  }
  delay(25);
}

/************************************/
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);
  printf("  \r \n");
  printf("Connecting to %s \n", ssid);
  int i = 0;
  bool Status = true;
  while (WiFi.status() != WL_CONNECTED) {
    if (Status) {
      digitalWrite(LED_1, 1);
      digitalWrite(LED_2, 1);
      delay(300);
      digitalWrite(LED_1, 0);
      digitalWrite(LED_2, 0);
      delay(300);
      Serial.print(i++);
      Serial.print(' ');
      if (i >= 20) {
        printf("Connection Failed!...\n");
        printf("A new connection attempt is made in 10 seconds \n");
        digitalWrite(LED_1, 1);
        digitalWrite(LED_2, 1);
        Status = false;
      }
    } else {
      delay(10000);
      ESP.restart();
    }
  }
  Serial.println("Conned strat! \n");
  Serial.print("IP address:  ");
  Serial.println(WiFi.localIP());
  delay(500);
}
void getTimer(void) {
  int hour = 8;
  int minute = 0;
  //Serial.println(timeClient.getFormattedTime());
  if (timeClient.isTimeSet()) {
    if (hour == timeClient.getHours() && minute == timeClient.getMinutes()) {
      digitalWrite(LED_2, HIGH);
    }
  }
  delay(500);
}
/***************/
