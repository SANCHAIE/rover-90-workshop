/* =====================================================================
   02 · DRIVE  (ฉบับเฉลย - สำหรับครูเท่านั้น)
   ---------------------------------------------------------------------
   ไฟล์นี้คือคำตอบของ 02_Drive_STUDENT
   อย่าเพิ่งแจกให้นักเรียนจนกว่าจะหมดเวลาช่วง coding
   ===================================================================== */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TEAM_NAME "TEAM 01"

#define ENA 14
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENB 32
#define BUZZER 4
#define BTN 0

int SPEED = 180;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  stopAll();

  Wire.begin(21, 22);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(SSD1306_WHITE);
  say("Press BOOT");
}

void loop() {
  if (digitalRead(BTN) == LOW) {
    delay(50);
    while (digitalRead(BTN) == LOW) { }
    countdown();
    mission();
    say("Done - press BOOT");
  }
}

/* ---------------- ภารกิจ : ไปแล้วกลับที่เดิม ---------------- */
void mission() {
  say("forward");   forward(SPEED);   delay(2000); stopAll(); delay(300);
  say("turn right"); turnRight(SPEED); delay(600);  stopAll(); delay(300);
  say("forward");   forward(SPEED);   delay(1000); stopAll(); delay(300);
  say("turn right"); turnRight(SPEED); delay(600);  stopAll(); delay(300);
  say("forward");   forward(SPEED);   delay(2000); stopAll();
  beep(2);
}

/* ---------------- ห้าฟังก์ชันที่นักเรียนต้องเขียนเอง ---------------- */
void forward(int s)   { leftMotor( 1, s); rightMotor( 1, s); }
void backward(int s)  { leftMotor(-1, s); rightMotor(-1, s); }
void turnLeft(int s)  { leftMotor(-1, s); rightMotor( 1, s); }
void turnRight(int s) { leftMotor( 1, s); rightMotor(-1, s); }
void stopAll()        { leftMotor( 0, 0); rightMotor( 0, 0); }

/* ---------------- ให้มาแล้ว ห้ามแก้ ---------------- */
void leftMotor(int dir, int speed) {
  digitalWrite(IN1, dir > 0);
  digitalWrite(IN2, dir < 0);
  analogWrite(ENA, dir == 0 ? 0 : speed);
}

void rightMotor(int dir, int speed) {
  digitalWrite(IN3, dir > 0);
  digitalWrite(IN4, dir < 0);
  analogWrite(ENB, dir == 0 ? 0 : speed);
}

void say(const char* msg) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println(TEAM_NAME);
  oled.setTextSize(2);
  oled.setCursor(0, 24);
  oled.println(msg);
  oled.setTextSize(1);
  oled.setCursor(0, 54);
  oled.print("speed ");
  oled.print(SPEED);
  oled.display();
  Serial.println(msg);
}

void countdown() {
  for (int i = 3; i > 0; i--) {
    char b[4];
    snprintf(b, sizeof(b), "%d", i);
    say(b);
    beep(1);
    delay(700);
  }
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH); delay(90);
    digitalWrite(BUZZER, LOW);  delay(120);
  }
}
