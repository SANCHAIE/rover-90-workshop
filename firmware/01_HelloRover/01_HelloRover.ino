/* =====================================================================
   01 · HELLO ROVER          นาทีที่ 28-33 ของกิจกรรม
   ---------------------------------------------------------------------
   โค้ดสั้นที่สุดที่พิสูจน์ว่า "ต่อสายถูกทุกเส้น"
   ห้ามอัปโหลดจนกว่าครูจะเซ็นชื่อในใบตรวจสาย

   เมื่ออัปโหลดสำเร็จ รถจะทำ 3 อย่างตามลำดับ
     1. จอ OLED ขึ้นชื่อทีม
     2. ล้อ "ซ้าย" หมุนไปข้างหน้า 1 วินาที      <-- ต้องเป็นล้อซ้ายก่อนเสมอ
     3. ล้อ "ขวา" หมุนไปข้างหน้า 1 วินาที
     4. buzzer ร้อง 2 ครั้ง แล้ววนใหม่

   ถ้าไม่เป็นแบบนี้ ดูตารางแก้ปัญหาท้ายไฟล์
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
#define LED_PIN 2

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  allStop();

  // ---------- เปิดจอ ----------
  Wire.begin(21, 22);                        // SDA = 21, SCL = 22
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED ไม่ตอบสนอง - ตรวจสาย SDA/SCL และไฟ 3.3V");
    while (true) {                            // กะพริบไฟถี่ ๆ แจ้งว่าจอมีปัญหา
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(150);
    }
  }

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(2);
  oled.setCursor(0, 8);
  oled.println(TEAM_NAME);
  oled.setTextSize(1);
  oled.setCursor(0, 40);
  oled.println("Wiring test");
  oled.println("Lift the wheels up!");
  oled.display();

  Serial.println("\n=== 01 HELLO ROVER ===");
  Serial.println("ยกล้อให้ลอยจากพื้นก่อน");
  delay(3000);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  showStep("LEFT wheel");
  leftMotor(1, 200);                 // ล้อซ้ายหมุนก่อนเสมอ
  delay(1000);
  allStop();
  delay(600);

  showStep("RIGHT wheel");
  rightMotor(1, 200);
  delay(1000);
  allStop();
  delay(600);

  showStep("OK - all good");
  beep(2);
  digitalWrite(LED_PIN, LOW);
  delay(2000);
}

/* ------------------------- ฟังก์ชันช่วย ------------------------- */
void showStep(const char* msg) {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0, 4);
  oled.println(TEAM_NAME);
  oled.setTextSize(1);
  oled.setCursor(0, 40);
  oled.println(msg);
  oled.display();
  Serial.println(msg);
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH); delay(90);
    digitalWrite(BUZZER, LOW);  delay(120);
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

void allStop() {
  leftMotor(0, 0);
  rightMotor(0, 0);
}

/* ---------------------------------------------------------------------
   อาการ                          สาเหตุ                    วิธีแก้
   ------------------------------------------------------------------
   จอไม่ติด ไฟกะพริบถี่           สาย SDA/SCL สลับกัน       สลับสองเส้นนี้
   จอไม่ติด ไฟไม่กะพริบเลย        ไม่มีไฟเข้าบอร์ด           วัดไฟที่ขา 5V ของ L298N
   ล้อขวาหมุนก่อนล้อซ้าย           สายควบคุมสลับข้าง          สลับ 14/27/26 กับ 32/25/33
   ล้อหมุนถอยหลัง                 สายมอเตอร์กลับขั้ว          สลับสายมอเตอร์ 2 เส้นของข้างนั้น
   ล้อไม่หมุนเลยทั้งสองข้าง        ลืมถอด jumper ครอบ ENA/ENB  ถอดออก
   หมุนแล้วบอร์ดรีเซ็ตวน           ถ่านอ่อน / ไฟตก            เปลี่ยนถ่านคู่ใหม่
   --------------------------------------------------------------------- */
