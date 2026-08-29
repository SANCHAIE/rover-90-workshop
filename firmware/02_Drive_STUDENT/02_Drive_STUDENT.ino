/* =====================================================================
   02 · DRIVE                นาทีที่ 33-50 ของกิจกรรม
   ---------------------------------------------------------------------
   ตอนนี้รถต่อสายเสร็จและอัปโหลดได้แล้ว ถึงเวลาสั่งให้มันวิ่ง

   งานของนักเรียนมี 2 ส่วน
     ส่วนที่ 1  เติมฟังก์ชัน 5 ตัวที่มีคำว่า TODO ให้ครบ  (นาทีที่ 33-40)
     ส่วนที่ 2  เขียนลำดับคำสั่งใน mission() ให้รถไปแล้วกลับที่เดิม (นาทีที่ 40-46)

   วิธีทดสอบ : ยกล้อลอย อัปโหลด แล้วกดปุ่ม BOOT บนบอร์ด
   ===================================================================== */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- แก้ตรงนี้ให้เป็นชื่อทีมของตัวเอง ----------
#define TEAM_NAME "TEAM 01"
// ----------------------------------------------------

#define ENA 14
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENB 32
#define BUZZER 4
#define BTN 0

int SPEED = 180;      // <<< นาทีที่ 46-50 ให้ลองเปลี่ยนเป็น 100 / 180 / 255

Adafruit_SSD1306 oled(128, 64, &Wire, -1);


/* =====================================================================
   ส่วนที่ 1 : เติมฟังก์ชัน 5 ตัวนี้ให้ครบ
   ---------------------------------------------------------------------
   เครื่องมือที่มีให้ใช้คือสองบรรทัดนี้

       leftMotor(ทิศทาง, ความเร็ว);
       rightMotor(ทิศทาง, ความเร็ว);

   ทิศทาง ใส่ได้ 3 ค่า      1 = หมุนไปข้างหน้า
                          -1 = หมุนถอยหลัง
                           0 = หยุด
   ความเร็ว ใส่ 0 ถึง 255

   ตัวอย่าง : ถ้าอยากให้ล้อซ้ายหมุนถอยหลังด้วยความเร็ว 150
       leftMotor(-1, 150);

   คำใบ้ : "หมุนขวาอยู่กับที่" คือล้อซ้ายเดินหน้า ล้อขวาถอยหลังพร้อมกัน
   ===================================================================== */

void forward(int s) {
  // TODO 1 : ทำให้ล้อทั้งสองข้างหมุนไปข้างหน้า


}

void backward(int s) {
  // TODO 2 : ทำให้ล้อทั้งสองข้างหมุนถอยหลัง


}

void turnLeft(int s) {
  // TODO 3 : หมุนซ้ายอยู่กับที่


}

void turnRight(int s) {
  // TODO 4 : หมุนขวาอยู่กับที่


}

void stopAll() {
  // TODO 5 : หยุดล้อทั้งสองข้าง


}


/* =====================================================================
   ส่วนที่ 2 : เขียนลำดับคำสั่งให้รถออกไปแล้วกลับมาที่เดิม
   ---------------------------------------------------------------------
   รูปแบบของหนึ่งคำสั่งคือ

       say("ข้อความขึ้นจอ");   ชื่อฟังก์ชัน(SPEED);   delay(เวลาเป็น ms);   stopAll();

   ตัวอย่างคำสั่งแรกให้ไว้แล้ว ที่เหลือเขียนต่อเอง
   ===================================================================== */

void mission() {
  say("forward");  forward(SPEED);  delay(2000);  stopAll();  delay(300);

  // TODO 6 : เขียนต่อให้รถเลี้ยว เดินหน้า แล้วกลับมาที่จุดเริ่มต้น




  beep(2);          // ร้อง 2 ครั้งเมื่อจบภารกิจ
}


/* =====================================================================
   ข้างล่างนี้เขียนมาให้แล้ว ไม่ต้องแก้
   ===================================================================== */

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
