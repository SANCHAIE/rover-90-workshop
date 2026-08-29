/* =====================================================================
   05 · SEND TO SHEET        นาทีที่ 78-86 ของกิจกรรม
   ---------------------------------------------------------------------
   ส่งผลงานที่จูนได้ขึ้นกราฟรวมของทั้งห้อง

   รถจะทำ 3 อย่างแล้วจบ ใช้เวลาราว 5 วินาที
       1. เชื่อมฮอตสปอตของครู
       2. ยิงข้อมูล 7 ค่าไปที่ Google Sheet
       3. ตัดการเชื่อมต่อทิ้งทันที

   ที่ต้องตัดการเชื่อมต่อทิ้ง เพราะฮอตสปอตมือถือรับได้แค่ 5-10 เครื่อง
   ถ้าทั้ง 15 ทีมค้างการเชื่อมต่อไว้พร้อมกัน จะไม่มีใครส่งได้เลย

   กดปุ่ม BOOT เพื่อส่ง  ส่งซ้ำได้ถ้าครั้งแรกไม่สำเร็จ
   ===================================================================== */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============ ครูตั้งค่า 3 บรรทัดนี้ให้เหมือนกันทุกเครื่อง ============
const char* WIFI_SSID = "RoverClass";
const char* WIFI_PASS = "rover2026";
const char* SCRIPT_URL =
  "https://script.google.com/macros/s/วางรหัสที่ได้จากการ Deploy ตรงนี้/exec";
// ==================================================================

// ============ นักเรียนกรอกผลงานของทีมตัวเอง ============
#define TEAM_NO   1        // หมายเลขทีม

float MS_PER_CM = 22.0;    // ค่าที่จูนได้จากไฟล์ 03
float TRIM_L    = 1.00;
float TRIM_R    = 1.00;

float ERR_1     = 0.0;     // คลาดเคลื่อนรอบที่ 1 ที่ระยะ 100 ซม. (ซม.)
float ERR_2     = 0.0;     // คลาดเคลื่อนรอบที่ 2
// ====================================================

#define TRIG 5
#define ECHO 18
#define BUZZER 4
#define BTN 0

const float SOUND_CM_PER_US = 0.0343;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  digitalWrite(TRIG, LOW);

  Wire.begin(21, 22);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(SSD1306_WHITE);

  WiFi.mode(WIFI_OFF);                 // ยังไม่ต้องเชื่อมจนกว่าจะกดปุ่ม

  screen("TEAM", String(TEAM_NO), "press BOOT to send");
  Serial.println("\n=== 05 SEND TO SHEET ===");
}

void loop() {
  if (digitalRead(BTN) == LOW) {
    delay(50);
    while (digitalRead(BTN) == LOW) { }
    sendData();
  }
  delay(50);
}

/* ------------------------- ส่งข้อมูล ------------------------- */
void sendData() {
  float dist = readDistance();          // อ่านระยะล่าสุดแนบไปด้วย

  // ---------- ขั้น 1 : เชื่อม WiFi ----------
  screen("STEP 1", "WiFi", "connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    screen("FAILED", "no WiFi", "check hotspot is on");
    longBeep();
    cleanUp();
    return;
  }
  Serial.printf("เชื่อมต่อแล้ว  IP %s  RSSI %d dBm\n",
                WiFi.localIP().toString().c_str(), WiFi.RSSI());

  // ---------- ขั้น 2 : ยิงข้อมูล ----------
  screen("STEP 2", "sending", "please wait");

  String url = String(SCRIPT_URL);
  url += "?team="    + String(TEAM_NO);
  url += "&mspercm=" + String(MS_PER_CM, 2);
  url += "&trimL="   + String(TRIM_L, 2);
  url += "&trimR="   + String(TRIM_R, 2);
  url += "&err1="    + String(ERR_1, 1);
  url += "&err2="    + String(ERR_2, 1);
  url += "&dist="    + String(dist, 1);
  Serial.println(url);

  WiFiClientSecure client;
  client.setInsecure();                 // ไม่ตรวจใบรับรอง พอสำหรับงานในห้องเรียน

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);   // Google เปลี่ยนเส้นทาง 1 ครั้ง
  http.setTimeout(12000);

  bool ok = false;
  if (http.begin(client, url)) {
    int code = http.GET();
    Serial.printf("HTTP %d\n", code);
    Serial.println(http.getString());
    ok = (code == 200);
    http.end();
  }

  // ---------- ขั้น 3 : ตัดการเชื่อมต่อ ----------
  cleanUp();

  if (ok) {
    screen("SUCCESS", "sent!", "look at the screen");
    beep(2);
  } else {
    screen("FAILED", "no reply", "press BOOT to retry");
    longBeep();
  }
}

void cleanUp() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);                  // คืนช่องให้ทีมถัดไป
  Serial.println("ตัดการเชื่อมต่อแล้ว");
}

/* ------------------------- เซนเซอร์ ------------------------- */
float readOnce() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long dur = pulseIn(ECHO, HIGH, 25000UL);
  if (dur == 0) return -1;
  return dur * SOUND_CM_PER_US / 2.0;
}

float readDistance() {
  float a = readOnce(); delay(12);
  float b = readOnce(); delay(12);
  float c = readOnce();
  if (a < 0 || b < 0 || c < 0) return -1;
  float hi = max(a, max(b, c));
  float lo = min(a, min(b, c));
  return a + b + c - hi - lo;
}

/* ------------------------- จอและเสียง ------------------------- */
void screen(const String& tag, const String& big, const String& note) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println(tag);
  oled.setTextSize(2);
  oled.setCursor(0, 20);
  oled.println(big);
  oled.setTextSize(1);
  oled.setCursor(0, 50);
  oled.println(note);
  oled.display();
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH); delay(90);
    digitalWrite(BUZZER, LOW);  delay(120);
  }
}

void longBeep() {
  digitalWrite(BUZZER, HIGH); delay(600);
  digitalWrite(BUZZER, LOW);
}

/* ---------------------------------------------------------------------
   แก้ปัญหา
     ค้างที่ STEP 1 ตลอด      ฮอตสปอตปิดอยู่ หรือชื่อ/รหัสไม่ตรง
                             ถ้าใช้ iPhone ให้เปิดหน้า Personal Hotspot ค้างไว้
     ขึ้น FAILED no reply     ยังไม่ได้ Deploy สคริปต์ หรือลิงก์ผิด
                             ตรวจว่าตอน Deploy ตั้ง Who has access = Anyone
     ส่งได้แต่ข้อมูลไม่ขึ้น    ชื่อชีตไม่ใช่ data หรือยังไม่ได้กด Authorize
   --------------------------------------------------------------------- */
