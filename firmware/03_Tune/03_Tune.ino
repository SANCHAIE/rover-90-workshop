/* =====================================================================
   03 · TUNE                 นาทีที่ 53-68 ของกิจกรรม
   ---------------------------------------------------------------------
   รถวิ่งได้แล้ว แต่ยังวิ่งไม่ "ตรงตามที่สั่ง"  ช่วงนี้คือการทำให้มันแม่น

   วิธีใช้ปุ่ม BOOT
       กดสั้น ๆ        = เริ่มวิ่งตามโหมดที่เลือกอยู่
       กดค้าง 1 วินาที = เปลี่ยนโหมด

   สามโหมดบนจอ
       [1] TEST 2s     วิ่งตรง 2000 ms   -> เอาไว้วัดว่าได้ระยะกี่ ซม.
       [2] GO 50cm     วิ่ง 50 ซม.       -> ทดสอบค่าที่คำนวณได้
       [3] GO 100cm    วิ่ง 100 ซม.      -> ภารกิจวัดผล ทำ 2 รอบ

   ลำดับการทำงาน
       ขั้น 1  โหมด [1] วิ่ง 3 ครั้ง วัดระยะทุกครั้ง หาค่าเฉลี่ย
       ขั้น 2  คำนวณ  MS_PER_CM = 2000 / ระยะเฉลี่ย(ซม.)   แล้วใส่ข้างล่าง
       ขั้น 3  โหมด [2] ทดสอบ ถ้าคลาดเคลื่อนมาก กลับไปแก้ MS_PER_CM
       ขั้น 4  รถเบี้ยว? ปรับ TRIM ทีละ 0.05
       ขั้น 5  โหมด [3] ทำ 2 รอบ จดความคลาดเคลื่อนลงใบงาน
   ===================================================================== */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================== ค่าที่นักเรียนต้องแก้ ==================
#define TEAM_NAME "TEAM 01"

int   SPEED      = 180;     // ใช้ค่าเดียวตลอด ห้ามเปลี่ยนกลางการทดลอง

float MS_PER_CM  = 22.0;    // <<< ขั้น 2 : ใส่ค่าที่คำนวณได้

float TRIM_L     = 1.00;    // <<< ขั้น 4 : รถเบี้ยวขวา ให้ลดค่านี้ทีละ 0.05
float TRIM_R     = 1.00;    // <<< ขั้น 4 : รถเบี้ยวซ้าย ให้ลดค่านี้ทีละ 0.05
// =========================================================

#define ENA 14
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENB 32
#define BUZZER 4
#define BTN 0

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

int mode = 0;                                   // 0,1,2
const char* MODE_NAME[3] = {"TEST 2s", "GO 50cm", "GO 100cm"};

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
  showMenu("ready");

  Serial.println("\n=== 03 TUNE ===");
}

void loop() {
  if (digitalRead(BTN) == LOW) {
    unsigned long t0 = millis();
    while (digitalRead(BTN) == LOW && millis() - t0 < 1200) { }

    if (millis() - t0 >= 1000) {                // กดค้าง = เปลี่ยนโหมด
      mode = (mode + 1) % 3;
      beep(1);
      showMenu("mode changed");
      while (digitalRead(BTN) == LOW) { }       // รอปล่อยปุ่ม
      delay(200);
    } else {                                    // กดสั้น = วิ่ง
      countdown();
      runMode();
      showMenu("done");
    }
  }
}

/* ------------------------- สามโหมด ------------------------- */
void runMode() {
  if (mode == 0) {
    showRun("2000 ms");
    forward(SPEED);
    delay(2000);
    stopAll();
  } else if (mode == 1) {
    goCm(50);
  } else {
    goCm(100);
  }
  beep(2);
}

// แปลงระยะทางเป็นเวลา แล้วสั่งวิ่ง
void goCm(float cm) {
  unsigned long ms = (unsigned long)(cm * MS_PER_CM);

  char line[24];
  snprintf(line, sizeof(line), "%d cm", (int)cm);
  showRun(line);
  Serial.printf("สั่งวิ่ง %.0f ซม. = %lu ms\n", cm, ms);

  forward(SPEED);
  delay(ms);
  stopAll();
}

/* ------------------------- มอเตอร์ ------------------------- */
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
void stopAll()      { leftMotor(0, 0); rightMotor(0, 0); }

/* ------------------------- จอและเสียง ------------------------- */
void showMenu(const char* status) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);   oled.println(TEAM_NAME);
  oled.setTextSize(2);
  oled.setCursor(0, 14);  oled.println(MODE_NAME[mode]);
  oled.setTextSize(1);
  oled.setCursor(0, 36);
  oled.print("ms/cm ");  oled.println(MS_PER_CM, 1);
  oled.setCursor(0, 46);
  oled.print("trim ");   oled.print(TRIM_L, 2);
  oled.print(" / ");     oled.println(TRIM_R, 2);
  oled.setCursor(0, 56);  oled.print(status);
  oled.display();
}

void showRun(const char* what) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);   oled.println("RUNNING");
  oled.setTextSize(2);
  oled.setCursor(0, 22);  oled.println(what);
  oled.display();
}

void countdown() {
  for (int i = 3; i > 0; i--) {
    oled.clearDisplay();
    oled.setTextSize(4);
    oled.setCursor(52, 18);
    oled.println(i);
    oled.display();
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

/* ---------------------------------------------------------------------
   คำถามชวนคิด (ตอบในใบงาน)
     - ทำไมรถแต่ละคันได้ค่า MS_PER_CM ไม่เท่ากัน ทั้งที่เป็นรุ่นเดียวกัน
     - ถ้าถ่านอ่อนลง ค่าที่จูนไว้วันนี้จะยังใช้ได้ไหม
     - ต้องเพิ่มอุปกรณ์อะไรเข้าไป ถึงจะไม่ต้องจูนใหม่ทุกครั้งที่เปลี่ยนถ่าน
   --------------------------------------------------------------------- */
