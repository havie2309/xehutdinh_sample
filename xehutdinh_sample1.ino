
#define BLYNK_TEMPLATE_ID "TMPL6aDWXU0U-"
#define BLYNK_TEMPLATE_NAME "XeHutDinh"
#define BLYNK_AUTH_TOKEN "auRqYYahg2VrFy4UzOQcDn8TD450Vtjf"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <AHT20.h>
#include <MQ135.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

BlynkTimer timer;


#define PIN_MQ135 36
MQ135 mq135_sensor(PIN_MQ135);
AHT20 aht20;
#define BLYNK_PRINT Serial

char ssid[] = "AS-TEACHER";
char pass[] = "66668888";

WiFiManager wifiManager;
int X, Y, Speed, Gatdinh, isRun;
int dir = 0;

const int in1 = 25;
const int in2 = 4;
const int in3 = 32;
const int in4 = 33;
const int in5 = 18;
const int in6 = 19;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int pwmChannel3 = 2;
const int pwmChannel4 = 3;
const int pwmChannel5 = 4;
const int pwmChannel6 = 5;
const int pwmFreq = 5000;
const int pwmResolution = 8;


int metalCount = 0;
bool lastMetalState = false;

#define THRESHOLD 50

BLYNK_WRITE(V0) {
  X = param.asInt();
}
BLYNK_WRITE(V1) {
  Y = param.asInt();
}
BLYNK_WRITE(V7) {
  Speed = param.asInt();
}
BLYNK_WRITE(V5) {
  Gatdinh = param.asInt();
}
BLYNK_WRITE(V6) {
  isRun = param.asInt();
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7);
}
void timeInterval() {
  String air;
  float t = aht20.getTemperature();
  float h = aht20.getHumidity();
  float correctedPPM = mq135_sensor.getCorrectedPPM(t, h);
  if (correctedPPM < 1000) {
    air = "good";
  } else if (correctedPPM >= 1000 && correctedPPM < 2000) {
    air = "fair";
  } else {
    air = "poor";
  }

  display.clearDisplay();

  // Hiển thị số lần phát hiện đinh
  display.setCursor(0, 5);
  display.print("Nail Count: ");
  display.print(metalCount);

  // Hiển thị nhiệt độ
  display.setCursor(0, 21);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");

  // Hiển thị độ ẩm
  display.setCursor(0, 37);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");

  // Hiển thị độ ẩm
  display.setCursor(0, 53);
  display.print("Air quality: ");
  display.print(air);



  display.display();


  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
}
void setup() {
  Serial.begin(115200);
  Wire.begin();
  aht20.begin();
  wifiManager.autoConnect("Xe Hut Dinh", "66668888");
  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setRotation(2);
  ledcSetup(pwmChannel1, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel3, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel4, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel5, pwmFreq, pwmResolution);
  ledcSetup(pwmChannel6, pwmFreq, pwmResolution);
  ledcAttachPin(in1, pwmChannel1);
  ledcAttachPin(in2, pwmChannel2);
  ledcAttachPin(in3, pwmChannel3);
  ledcAttachPin(in4, pwmChannel4);
  ledcAttachPin(in5, pwmChannel5);
  ledcAttachPin(in6, pwmChannel6);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(39, INPUT);
  pinMode(36, INPUT);

  timer.setInterval(15000L, timeInterval);
}

void loop() {
  Blynk.run();
  timer.run();
  updateValues();
  if (Gatdinh == true) {
    hutdinh();
  } else {
    thadinh();
  }
  // int count1_35 = 0;
  // int count0_35 = 0;

  // for (int i = 0; i < 5; i++) {
  //   int value35 = digitalRead(35);

  //   if (value35 ==1) {
  //     count1_35++;
  //   } else {
  //     count0_35++;
  //   }

  //delay(10);
  //debug1();


  // bool metalDetected = count1_35 > count0_35;

  // if (metalDetected) {
  //   if (!lastMetalState) {
  //     metalCount++;
  //     Serial.println(metalCount);
  //     lastMetalState = true;
  //   }
  // } else {
  //   lastMetalState = false;
  // }
}

void updateValues() {
  if (isRun == true) {
    if (Y > THRESHOLD && abs(X) < THRESHOLD) {
      tienthang(Speed);
    } else if (Y < -THRESHOLD && abs(X) < THRESHOLD) {
      luithang(Speed);
    } else if (X < -THRESHOLD && abs(Y) < THRESHOLD) {
      retrai(Speed);
    } else if (X > THRESHOLD && abs(Y) < THRESHOLD) {
      rephai(Speed);
    } else if (X < -THRESHOLD && Y > THRESHOLD) {
      luivongphai(Speed);
    } else if (X > THRESHOLD && Y > THRESHOLD) {

      luivongtrai(Speed);
    } else if (X < -THRESHOLD && Y < -THRESHOLD) {
      tienvongphai(Speed);
    } else if (X > THRESHOLD && Y < -THRESHOLD) {

      tienvongtrai(Speed);
    } else {
      stop();
    }
  }
}

void tienthang(int speed) {
  ledcWrite(pwmChannel1, speed);
  ledcWrite(pwmChannel2, 0);
  ledcWrite(pwmChannel3, speed);
  ledcWrite(pwmChannel4, 0);
}

void luithang(int speed) {
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, speed);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, speed);
}

void retrai(int speed) {
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, speed / 2);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, 0);
}

void rephai(int speed) {
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, 0);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, speed / 2);
}

void tienvongphai(int speed) {
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, speed / 2);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, speed);
}

void tienvongtrai(int speed) {
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, speed);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, speed / 2);
}

void luivongtrai(int speed) {
  ledcWrite(pwmChannel1, speed);
  ledcWrite(pwmChannel2, 0);
  ledcWrite(pwmChannel3, speed / 2);
  ledcWrite(pwmChannel4, 0);
}

void luivongphai(int speed) {
  ledcWrite(pwmChannel1, speed / 2);
  ledcWrite(pwmChannel2, 0);
  ledcWrite(pwmChannel3, speed);
  ledcWrite(pwmChannel4, 0);
}

void stop() {
  Serial.println("dừng");
  ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, 0);
  ledcWrite(pwmChannel3, 0);
  ledcWrite(pwmChannel4, 0);
}


void hutdinh() {
  ledcWrite(pwmChannel5, 0);
  ledcWrite(pwmChannel6, 255);
}
void thadinh() {
  ledcWrite(pwmChannel5, 255);
  ledcWrite(pwmChannel6, 0);
}


void debug1() {
  // int value34 = digitalRead(34);  // Đọc giá trị của chân 34
  int value35 = analogRead(35);  // Đọc giá trị của chân 35
  //int value39 = digitalRead(39);  // Đọc giá trị của chân 39
  if (value35 == 4095) {
    Serial.println("1");

  } else {
    Serial.println("0");
  }
  // In giá trị ra Serial
  /// Serial.print("Pin 34: ");
  // Serial.print(value34);

  // Serial.print(" | Pin 39: ");
  //Serial.println(value39);

  //delay(1000);  // Đợi 1 giây trước khi đọc lại
}