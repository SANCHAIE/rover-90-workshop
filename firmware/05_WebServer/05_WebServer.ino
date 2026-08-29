/* =====================================================================
   05 · WEB SERVER           นาทีที่ 78-86 ของกิจกรรม
   ---------------------------------------------------------------------
   IoT แบบไม่ต้องพึ่งอะไรเลย  ไม่ต้องมีเราเตอร์ ไม่ต้องมีเน็ต ไม่ต้องมีฮอตสปอต
   ตัว ESP32 ปล่อย WiFi ของตัวเองและเป็นเว็บเซิร์ฟเวอร์ในตัว

   วิธีใช้
       1. กดปุ่ม BOOT ค้าง 1 วินาที   -> เปิด WiFi ของรถ จอจะขึ้นชื่อ WiFi
       2. มือถือเชื่อม WiFi ชื่อ ROVER-xx  รหัส rover2026
       3. เปิดเบราว์เซอร์พิมพ์  192.168.4.1   (ทุกทีมใช้เลขนี้เหมือนกันหมด)
       4. กดปุ่ม BOOT สั้น ๆ           -> ปิด WiFi คืนคลื่นให้ทีมถัดไป

   *** เหตุผลที่ต้องเปิด-ปิดเอง ***
   ถ้าทั้ง 15 ทีมเปิด WiFi พร้อมกัน คลื่น 2.4 GHz จะแน่นจนเชื่อมไม่ติดสักทีม
   ครูจึงควรเรียกทีละ 3-4 ทีมขึ้นมาเปิด ทดลอง แล้วปิดก่อนเรียกกลุ่มถัดไป
   โค้ดนี้ปิด WiFi ให้อัตโนมัติหลังเปิดครบ 3 นาทีด้วย กันเด็กลืมปิด
   ===================================================================== */

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================== นักเรียนกรอกของทีมตัวเอง ==================
#define TEAM_NO   1          // <<< หมายเลขทีม จะกลายเป็นชื่อ WiFi ROVER-01

float MS_PER_CM = 22.0;      // ค่าที่จูนได้จากไฟล์ 03
float TRIM_L    = 1.00;
float TRIM_R    = 1.00;
float ERR_1     = 0.0;       // คลาดเคลื่อนรอบที่ 1 ที่ระยะ 100 ซม.
float ERR_2     = 0.0;       // คลาดเคลื่อนรอบที่ 2
// ===========================================================

// ================== ครูปรับได้ ==================
const char* AP_PASS   = "rover2026";
#define AP_MINUTES     3          // เปิด WiFi ได้นานกี่นาทีก่อนปิดเอง
#define ALLOW_DRIVING  1          // 0 = ปิดปุ่มบังคับรถ เหลือแค่หน้าจอข้อมูล
// ==============================================

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
const unsigned long CMD_TIMEOUT = 600;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
WebServer server(80);

bool  apOn = false;
unsigned long apStart = 0;
float distance = -1;
unsigned long lastSense = 0;
unsigned long lastDraw  = 0;
char  cmd = 'S';
unsigned long lastCmd = 0;
int   speedNow = 170;

char ssid[16];

/* =====================================================================
   หน้าเว็บ  วาดกราฟด้วย canvas ไม่ต้องโหลดไลบรารีจากอินเทอร์เน็ต
   (สำคัญ เพราะรถไม่ได้ต่อเน็ต โหลดอะไรจากข้างนอกไม่ได้เลย)
   ===================================================================== */
const char PAGE[] PROGMEM = R"HTML(<!DOCTYPE html><html lang="th"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>Rover</title><style>
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;padding:16px;font-family:system-ui,-apple-system,sans-serif;
background:#0f1318;color:#e8ecf1;text-align:center;user-select:none}
h1{margin:0;font-size:20px;letter-spacing:.04em}
.sub{color:#7d8894;font-size:13px;margin-bottom:14px}
.big{font-size:52px;font-weight:700;line-height:1;margin:10px 0 2px;
font-variant-numeric:tabular-nums}
.big small{font-size:15px;font-weight:400;color:#7d8894;margin-left:4px}
.ok{color:#4ade80}.warn{color:#ff6b6b}
canvas{width:100%;max-width:420px;height:110px;background:#161c23;
border-radius:8px;margin:10px 0}
.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(96px,1fr));
gap:8px;max-width:420px;margin:0 auto 16px}
.c{background:#161c23;border-radius:8px;padding:9px 6px}
.c b{display:block;font-size:10px;letter-spacing:.1em;color:#7d8894;
text-transform:uppercase;margin-bottom:3px}
.c span{font-size:17px;font-weight:600;font-variant-numeric:tabular-nums}
.pad{display:grid;grid-template-columns:repeat(3,1fr);gap:9px;
max-width:300px;margin:0 auto}
button{font-size:24px;padding:18px 0;border:0;border-radius:12px;
background:#252d38;color:#e8ecf1;font-weight:700;touch-action:none}
button:active{background:#3f7fd6}
.stop{background:#9e2f2f;grid-column:2}
.t{color:#7d8894;font-size:12px;margin-top:14px}
</style></head><body>

<h1>ROVER __TEAM__</h1>
<div class="sub">192.168.4.1</div>

<div class="big"><span id="d">--</span><small>ซม.</small></div>
<div id="st" class="sub">กำลังเชื่อมต่อ...</div>
<canvas id="g" width="420" height="110"></canvas>

<div class="cards">
  <div class="c"><b>ms/cm</b><span>__MSPCM__</span></div>
  <div class="c"><b>trim L</b><span>__TL__</span></div>
  <div class="c"><b>trim R</b><span>__TR__</span></div>
  <div class="c"><b>err 1</b><span>__E1__</span></div>
  <div class="c"><b>err 2</b><span>__E2__</span></div>
</div>

__PAD__

<div class="t">ปิด WiFi อัตโนมัติใน <span id="left">--</span> วินาที</div>

<script>
var hist=[],cv=document.getElementById('g'),cx=cv.getContext('2d');
function draw(){
  var w=cv.width,h=cv.height;
  cx.clearRect(0,0,w,h);
  cx.strokeStyle='#252d38';cx.lineWidth=1;
  for(var i=1;i<4;i++){var y=h*i/4;cx.beginPath();cx.moveTo(0,y);cx.lineTo(w,y);cx.stroke();}
  if(hist.length<2)return;
  var n=hist.length,step=w/59;
  cx.beginPath();
  for(var j=0;j<n;j++){
    var v=hist[j];if(v<0)v=0;if(v>80)v=80;
    var x=j*step,y=h-(v/80)*h;
    j?cx.lineTo(x,y):cx.moveTo(x,y);
  }
  cx.strokeStyle='#4ade80';cx.lineWidth=2;cx.stroke();
  var last=hist[n-1];
  if(last>=0){
    var lx=(n-1)*step,ly=h-(Math.min(last,80)/80)*h;
    cx.fillStyle='#4ade80';cx.beginPath();cx.arc(lx,ly,3.5,0,7);cx.fill();
  }
}
function poll(){
  fetch('/d').then(function(r){return r.json()}).then(function(j){
    var d=document.getElementById('d'),s=document.getElementById('st');
    d.textContent=j.cm<0?'--':j.cm.toFixed(1);
    d.className=j.cm>=0&&j.cm<15?'warn':'ok';
    s.textContent=j.cm<0?'ไม่พบวัตถุข้างหน้า':'กำลังวัดอยู่';
    document.getElementById('left').textContent=j.left;
    hist.push(j.cm);if(hist.length>60)hist.shift();
    draw();
  }).catch(function(){document.getElementById('st').textContent='ขาดการเชื่อมต่อ'});
}
setInterval(poll,300);poll();

var held=null,timer=null;
function send(c){fetch('/c?m='+c).catch(function(){})}
document.querySelectorAll('button').forEach(function(b){
  var c=b.dataset.c;
  function dn(e){e.preventDefault();held=c;send(c);
    clearInterval(timer);timer=setInterval(function(){send(held)},200)}
  function up(e){if(e&&e.preventDefault)e.preventDefault();
    held=null;clearInterval(timer);send('S')}
  b.addEventListener('touchstart',dn,{passive:false});
  b.addEventListener('touchend',up);
  b.addEventListener('touchcancel',up);
  b.addEventListener('mousedown',dn);
  b.addEventListener('mouseup',up);
  b.addEventListener('mouseleave',function(){if(held)up()});
});
</script></body></html>)HTML";

const char PAD_HTML[] PROGMEM =
  "<div class=\"pad\">"
  "<div></div><button data-c=\"F\">&#9650;</button><div></div>"
  "<button data-c=\"L\">&#9664;</button>"
  "<button class=\"stop\" data-c=\"S\">&#9632;</button>"
  "<button data-c=\"R\">&#9654;</button>"
  "<div></div><button data-c=\"B\">&#9660;</button><div></div>"
  "</div>";

/* ===================================================================== */
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

  snprintf(ssid, sizeof(ssid), "ROVER-%02d", TEAM_NO);

  WiFi.mode(WIFI_OFF);                     // ปิดไว้ก่อน อย่าเพิ่งกวนคลื่น
  showOff();

  server.on("/",  handleRoot);
  server.on("/d", handleData);
  server.on("/c", handleCmd);

  Serial.printf("\n=== 05 WEB SERVER (%s) ===\n", ssid);
  Serial.println("กดปุ่ม BOOT ค้าง 1 วินาที เพื่อเปิด WiFi");
}

void loop() {
  handleButton();

  if (millis() - lastSense > 130) {
    lastSense = millis();
    distance = readDistance();
  }

  if (apOn) {
    server.handleClient();

    if (cmd != 'S' && millis() - lastCmd > CMD_TIMEOUT) cmd = 'S';
    applyMotors();

    if (millis() - apStart > (unsigned long)AP_MINUTES * 60000UL) {
      stopAP();                            // หมดเวลา ปิดเอง
      beep(3);
    } else if (millis() - lastDraw > 1000) {
      lastDraw = millis();                 // วาดจอวินาทีละครั้งพอ
      showOn();                            // วาดถี่กว่านี้จะไปแย่งเวลาของเว็บเซิร์ฟเวอร์
    }
  } else if (millis() - lastDraw > 1000) {
    lastDraw = millis();
    showOff();
  }
}

/* ------------------------- ปุ่ม ------------------------- */
void handleButton() {
  if (digitalRead(BTN) != LOW) return;

  unsigned long t0 = millis();
  while (digitalRead(BTN) == LOW && millis() - t0 < 1200) { }
  bool longPress = (millis() - t0 >= 1000);
  while (digitalRead(BTN) == LOW) { }
  delay(150);

  if (longPress && !apOn)  startAP();
  else if (!longPress && apOn) { stopAP(); beep(1); }
}

/* ------------------------- เปิด/ปิด WiFi ------------------------- */
void startAP() {
  // กระจายช่องสัญญาณ ทีม 1-5 ใช้ช่อง 1, ทีม 6-10 ใช้ช่อง 6, ที่เหลือช่อง 11
  int ch = (TEAM_NO <= 5) ? 1 : (TEAM_NO <= 10 ? 6 : 11);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, AP_PASS, ch, 0, 2);    // ให้ต่อได้แค่ 2 เครื่อง
  WiFi.setTxPower(WIFI_POWER_8_5dBm);      // ลดกำลังส่ง กันชนกับทีมข้าง ๆ

  server.begin();
  apOn = true;
  apStart = millis();
  beep(2);

  Serial.printf("เปิด WiFi %s ช่อง %d  IP %s\n",
                ssid, ch, WiFi.softAPIP().toString().c_str());
  showOn();
}

void stopAP() {
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  apOn = false;
  cmd = 'S';
  stopAll();
  Serial.println("ปิด WiFi แล้ว คืนคลื่นให้ทีมถัดไป");
  showOff();
}

/* ------------------------- หน้าเว็บ ------------------------- */
void handleRoot() {
  String html = FPSTR(PAGE);
  char t[8];
  snprintf(t, sizeof(t), "%02d", TEAM_NO);
  html.replace("__TEAM__",  String(t));
  html.replace("__MSPCM__", String(MS_PER_CM, 1));
  html.replace("__TL__",    String(TRIM_L, 2));
  html.replace("__TR__",    String(TRIM_R, 2));
  html.replace("__E1__",    String(ERR_1, 1));
  html.replace("__E2__",    String(ERR_2, 1));
#if ALLOW_DRIVING
  html.replace("__PAD__", FPSTR(PAD_HTML));
#else
  html.replace("__PAD__", "");
#endif
  server.send(200, "text/html; charset=utf-8", html);
}

void handleData() {
  long left = (long)AP_MINUTES * 60L - (long)((millis() - apStart) / 1000UL);
  if (left < 0) left = 0;
  String j = "{\"cm\":" + String(distance, 1) + ",\"left\":" + String(left) + "}";
  server.send(200, "application/json", j);
}

void handleCmd() {
#if ALLOW_DRIVING
  if (server.hasArg("m")) {
    char c = server.arg("m").charAt(0);
    if (c == 'F' || c == 'B' || c == 'L' || c == 'R' || c == 'S') {
      cmd = c;
      lastCmd = millis();
    }
  }
#endif
  server.send(200, "text/plain", "ok");
}

/* ------------------------- มอเตอร์ ------------------------- */
void applyMotors() {
  char c = cmd;
  if (distance > 0 && distance < 12 && c == 'F') c = 'S';   // เบรกอัตโนมัติ

  switch (c) {
    case 'F': leftMotor( 1, speedNow); rightMotor( 1, speedNow); break;
    case 'B': leftMotor(-1, speedNow); rightMotor(-1, speedNow); break;
    case 'L': leftMotor(-1, speedNow); rightMotor( 1, speedNow); break;
    case 'R': leftMotor( 1, speedNow); rightMotor(-1, speedNow); break;
    default:  stopAll();                                         break;
  }
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

void stopAll() { leftMotor(0, 0); rightMotor(0, 0); }

/* ------------------------- เซนเซอร์ ------------------------- */
float readOnce() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long dur = pulseIn(ECHO, HIGH, 20000UL);
  if (dur == 0) return -1;
  return dur * SOUND_CM_PER_US / 2.0;
}

float readDistance() { return readOnce(); }

/* ------------------------- จอ ------------------------- */
void showOn() {
  long left = (long)AP_MINUTES * 60L - (long)((millis() - apStart) / 1000UL);
  if (left < 0) left = 0;

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);   oled.println("WiFi ON");
  oled.setTextSize(2);
  oled.setCursor(0, 12);  oled.println(ssid);
  oled.setTextSize(1);
  oled.setCursor(0, 34);  oled.println(AP_PASS);
  oled.setCursor(0, 44);  oled.println("192.168.4.1");
  oled.setCursor(0, 56);
  oled.print("off in ");  oled.print(left); oled.print("s");
  oled.display();
}

void showOff() {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0, 6);   oled.println(ssid);
  oled.setTextSize(1);
  oled.setCursor(0, 30);  oled.println("WiFi OFF");
  oled.setCursor(0, 44);  oled.println("hold BOOT 1s");
  oled.setCursor(0, 54);  oled.println("to turn on");
  oled.display();
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH); delay(90);
    digitalWrite(BUZZER, LOW);  delay(120);
  }
}

/* ---------------------------------------------------------------------
   คำถามชวนคิด
     - ทำไมทุกทีมใช้เลข 192.168.4.1 เหมือนกันได้ ไม่ชนกันหรือ
     - ถ้าเปิด WiFi พร้อมกันทั้ง 15 ทีมจะเกิดอะไรขึ้น ลองดูได้ถ้ามีเวลา
     - รถไม่ได้ต่ออินเทอร์เน็ตเลย แบบนี้เรียกว่า IoT ได้ไหม เพราะอะไร
   --------------------------------------------------------------------- */
