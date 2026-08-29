/* =====================================================================
   04 · SENSE                นาทีที่ 68-78 ของกิจกรรม
   ---------------------------------------------------------------------
   จนถึงตอนนี้รถทำตามคำสั่งอย่างเดียว ยังไม่เคย "รู้" อะไรเกี่ยวกับโลกรอบตัว
   ช่วงนี้ใส่ตาให้มัน

   หลักการ  เซนเซอร์ยิงคลื่นเสียงออกไป จับเวลาที่สะท้อนกลับ
            เสียงเดินทาง 343 เมตร/วินาที = 0.0343 ซม. ต่อไมโครวินาที
            เวลาที่วัดได้คือไป-กลับ จึงต้องหาร 2

                ระยะ (ซม.) = เวลา (us) x 0.0343 / 2

   ปุ่ม BOOT
       กดสั้น ๆ        = เริ่มภารกิจ "ขับเข้าหากำแพงแล้วหยุดเอง"
       กดค้าง 1 วินาที = สลับเป็นโหมดอ่านค่าเฉย ๆ (ไว้เทียบไม้บรรทัด)
   ===================================================================== */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================== ค่าที่นักเรียนแก้ได้ ==================
#define TEAM_NAME "TEAM 01"

int   SPEED     = 150;      // ความเร็วตอนขับเข้าหากำแพง
float STOP_CM   = 15.0;     // <<< หยุดเมื่อใกล้กว่ากี่ ซม.

float TRIM_L    = 1.00;     // ใส่ค่าที่จูนได้จากไฟล์ 03
float TRIM_R    = 1.00;
// =========================================================

#define ENA 14
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENB 32
#define TRIG 5
#define ECHO 18
#define BUZZER 4
#define BTN 0

const float SOUND_CM_PER_US = 0.0343;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool readOnlyMode = true;          // เริ่มที่โหมดอ่านค่า
unsigned long lastBeep = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  digitalWrite(TRIG, LOW);
  stopAll();

  Wire.begin(21, 22);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(SSD1306_WHITE);

  Serial.println("\n=== 04 SENSE ===");
}

void loop() {
  float d = readDistance();
  showDistance(d);
  beepByDistance(d);

  if (digitalRead(BTN) == LOW) {
    unsigned long t0 = millis();
    while (digitalRead(BTN) == LOW && millis() - t0 < 1200) { }

    if (millis() - t0 >= 1000) {              // กดค้าง = สลับโหมด
      readOnlyMode = !readOnlyMode;
      beep(1);
      while (digitalRead(BTN) == LOW) { }
      delay(200);
    } else if (!readOnlyMode) {               // กดสั้น = ทำภารกิจ
      driveUntilWall();
    }
  }

  delay(120);
}

/* ------------- ภารกิจ : ขับเข้าหากำแพงแล้วหยุดเอง ------------- */
void driveUntilWall() {
  unsigned long t0 = millis();
  forward(SPEED);

  while (true) {
    float d = readDistance();
    showDistance(d);

    // เงื่อนไขเดียวที่ทำให้หุ่นยนต์ "ตัดสินใจ" ได้
    if (d > 0 && d < STOP_CM) {
      stopAll();
      beep(3);
      Serial.printf("หยุดที่ %.1f ซม.\n", d);
      return;
    }

    // ความปลอดภัย : วิ่งเกิน 8 วินาทีให้หยุดเอง
    if (millis() - t0 > 8000) {
      stopAll();
      Serial.println("หมดเวลา - หยุด");
      return;
    }
    delay(40);
  }
}

/* ------------------------- อ่านเซนเซอร์ ------------------------- */
float readOnce() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long dur = pulseIn(ECHO, HIGH, 25000UL);
  if (dur == 0) return -1;
  return dur * SOUND_CM_PER_US / 2.0;
}

// อ่าน 3 ครั้งแล้วเอาค่ากลาง ตัดค่าหลุดทิ้ง
float readDistance() {
  float a = readOnce(); delay(12);
  float b = readOnce(); delay(12);
  float c = readOnce();
  if (a < 0 || b < 0 || c < 0) return -1;
  float hi = max(a, max(b, c));
  float lo = min(a, min(b, c));
  return a + b + c - hi - lo;
}

/* ------------------------- เสียงเตือน ------------------------- */
// ยิ่งใกล้ยิ่งร้องถี่  เหมือนเซนเซอร์ถอยหลังในรถยนต์
void beepByDistance(float d) {
  if (d < 0 || d > 60) return;

  unsigned long gap;
  if (d < STOP_CM)      gap = 0;          // ใกล้มาก = ร้องยาว
  else if (d < 25)      gap = 120;
  else if (d < 40)      gap = 350;
  else                  gap = 700;

  if (gap == 0) {
    digitalWrite(BUZZER, HIGH);
    return;
  }
  if (millis() - lastBeep > gap) {
    lastBeep = millis();
    digitalWrite(BUZZER, HIGH); delay(25);
    digitalWrite(BUZZER, LOW);
  }
}

/* ------------------------- จอ ------------------------- */
void showDistance(float d) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(TEAM_NAME);
  oled.setCursor(78, 0);
  oled.print(readOnlyMode ? "READ" : "DRIVE");

  oled.setTextSize(3);
  oled.setCursor(0, 18);
  if (d < 0) oled.print("---");
  else       oled.print(d, 1);

  oled.setTextSize(1);
  oled.setCursor(96, 32);
  oled.print("cm");

  // แถบกราฟ 0-60 ซม.
  int w = (d < 0) ? 0 : (int)(d / 60.0 * 126.0);
  if (w > 126) w = 126;
  oled.drawRect(0, 50, 128, 10, SSD1306_WHITE);
  oled.fillRect(1, 51, w, 8, SSD1306_WHITE);
  oled.display();
}

void leftMotor(int dir, int speed) {
  digitalWrite(IN1, dir > 0);
  digitalWrite(IN2, dir < 0);
  analogWrite(ENA, dir == 0 ? 0 : (int)(speed * TRIM_L));
}

void rightMotor(int dir, int speed) {
  digitalWrite(IN3, dir > 0);
  digitalWrite(IN4, dir < 0);
  analogWrite(ENB, dir == 0 ? 0 : (int)(speed * TRIM_R));
}

void forward(int s) { leftMotor(1, s); rightMotor(1, s); }
void stopAll()      { leftMotor(0, 0); rightMotor(0, 0); digitalWrite(BUZZER, LOW); }

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH); delay(90);
    digitalWrite(BUZZER, LOW);  delay(120);
  }
}

/* ---------------------------------------------------------------------
   คำถามชวนคิด
     - ลองวัดผ้าหรือฟองน้ำ ทำไมค่าถึงเพี้ยน
     - ถ้าลด STOP_CM เหลือ 5 ซม. รถจะหยุดทันไหม เพราะอะไร
     - เซนเซอร์นี้มองเห็นมุมกว้างราว 15 องศา มันจะ "มองไม่เห็น" อะไรบ้าง
   --------------------------------------------------------------------- */
