# Prompt สำหรับสร้างรูปประกอบ Rover 90

ใช้กับเครื่องมือสร้างภาพ เช่น Midjourney, DALL·E, Imagen, Flux
เขียนเป็นภาษาอังกฤษเพราะโมเดลส่วนใหญ่เข้าใจได้แม่นกว่า

---

## อ่านก่อนใช้ · รูปไหนใช้ AI ได้ รูปไหนห้าม

โมเดลสร้างภาพ **วาดสายไฟผิดเสมอ** มันสร้างภาพที่ "ดูเหมือนวงจร" แต่สายจะไปผิดขา จำนวนขาผิด และตัวอักษรบนบอร์ดจะเป็นตัวมั่ว ถ้าเอารูปพวกนี้ไปเป็นคู่มือให้เด็กต่อสาย เด็กจะต่อผิดตามและของพัง

| รูป | ใช้ AI ได้ไหม | เหตุผล |
|---|---|---|
| 1 · ทั้งคันจากด้านบน | ใช้ได้ เป็นภาพเปิดหรือปกเท่านั้น | ห้ามใช้อ้างอิงการต่อสาย |
| 2 · ขั้วสกรู L298N | **ห้าม** | จุดเดียวที่ต่อผิดแล้วไหม้ ต้องถูก 100% |
| 3 · แถวขา ESP32 | **ห้าม** | ตัวอักษรบนบอร์ดต้องอ่านออกและถูกต้อง AI เขียนเป็นตัวมั่ว |
| 4 · ตัวแบ่งแรงดัน | **ห้าม** | แถบสีตัวต้านทานจะผิด เด็กจะหยิบค่าผิด |
| 5 · จอและ buzzer ที่ติดตั้งแล้ว | ใช้ได้ ถ้าใช้ดูแนวการจัดวางเฉย ๆ | |
| 6 · รถที่เปิดสวิตช์แล้วจอติด | ใช้ได้ เป็นภาพเป้าหมาย | |

**สรุป** รูป 2 3 4 ต้องถ่ายจากรถต้นแบบจริงเท่านั้น ไม่มีทางลัด
ส่วนรูป 1 5 6 ใช้ AI สร้างไปก่อนได้ แล้วค่อยเปลี่ยนเป็นรูปจริงทีหลัง

---

## Negative prompt · ใส่ทุกครั้ง

```
text, letters, labels, watermark, logo, hands, people, blurry,
distorted electronics, extra wires, tangled cables, fisheye,
oversaturated, cartoon, 3d render, illustration
```

ที่ต้องใส่ `text, letters, labels` เพราะโมเดลจะพยายามเขียนตัวหนังสือบนบอร์ดแล้วออกมาเป็นตัวมั่วทุกครั้ง สู้ให้มันไม่เขียนเลยแล้วเราไปใส่ป้ายกำกับเองทีหลังดีกว่า

**ตั้งค่าที่แนะนำ** อัตราส่วน 4:3 หรือ 3:2 · ความละเอียดสูงสุดที่ทำได้ · ถ้ามีตัวเลือก style ให้เลือก photographic หรือ raw

---

## รูปที่ 1 · ทั้งคันจากด้านบน

```
Top-down flat lay photograph of a small two-wheel robot car chassis for
education, transparent acrylic base plate, two yellow TT gear motors with
black rubber wheels, a green L298N motor driver board with black aluminium
heatsink, a black ESP32 development board, a small blue ultrasonic distance
sensor with two silver cylindrical transducers mounted at the front, a tiny
blue OLED display module, colourful silicone jumper wires in red black
orange blue green neatly routed, a black 18650 battery holder underneath,
placed on a clean light grey studio surface, soft diffused overhead
lighting, no harsh shadows, shot on 50mm lens at f/8, sharp focus edge to
edge, product photography, high detail
```

---

## รูปที่ 2 · ขั้วสกรู L298N ระยะใกล้ · ต้องถ่ายจริง

ถ้าจะลองสร้างดูเพื่อเอาไปเทียบมุมกล้อง

```
Extreme close-up macro photograph of a green L298N dual H-bridge motor
driver module, focus on the blue screw terminal block, one thick red wire
and one thick black wire clamped into adjacent terminals, small yellow
jumper cap seated on a two-pin header nearby, black aluminium heatsink
visible behind, shallow depth of field, 100mm macro lens at f/5.6, soft
side lighting from the left, clean neutral background, hyper detailed,
visible screw heads and solder joints
```

---

## รูปที่ 3 · แถวขา ESP32 ระยะใกล้ · ต้องถ่ายจริง

```
Macro photograph of a black ESP32 development board lying flat, camera
angled low along the row of male header pins, several coloured dupont
jumper wires plugged onto consecutive pins, silver metal RF shield can
visible on the board, micro USB connector at the far end, shallow depth of
field with the pin row in sharp focus, 100mm macro lens, soft diffused
lighting, clean white surface, product photography
```

---

## รูปที่ 4 · ตัวแบ่งแรงดัน · ต้องถ่ายจริง

```
Macro photograph of two small axial resistors soldered inline between
jumper wires, beige resistor bodies with coloured stripe bands, joints
covered with clear heat shrink tubing, wires continue to a dupont
connector, resting on a light grey work surface, 100mm macro lens at f/6,
soft even lighting, extremely sharp, electronics repair photography
```

---

## รูปที่ 5 · จอและ buzzer ที่ติดตั้งแล้ว

```
Close-up photograph of a small blue OLED display module mounted upright on
a robot car chassis, the dark glass panel facing the camera, a small black
cylindrical buzzer mounted beside it, thin coloured wires running down to
the chassis, soft indoor daylight from a window, shallow depth of field
with a blurred classroom in the background, 35mm lens at f/2.8,
documentary photography style
```

---

## รูปที่ 6 · รถที่เปิดสวิตช์แล้วจอติด

```
Photograph of a small educational robot car standing on a wooden classroom
desk, its tiny OLED screen glowing with cyan text, an ultrasonic sensor at
the front looking like two round eyes, colourful wires visible on top of
the chassis, warm afternoon light from a window on the left, shallow depth
of field with a softly blurred classroom behind, 35mm lens at f/2, warm and
inviting mood, documentary photography
```

---

## โบนัส · ภาพปกสำหรับสไลด์หรือ README

ภาพแบบนี้ AI ทำได้ดีที่สุด เพราะไม่ต้องแม่นเรื่องวงจร

```
Wide cinematic photograph of three small educational robot cars lined up on
a long wooden desk in a bright classroom, shallow depth of field with the
nearest car in sharp focus and the others softly blurred, colourful jumper
wires and tiny glowing screens, warm natural window light from the right,
clean uncluttered composition with generous empty space on the left for a
title, 35mm lens at f/2, editorial photography, muted natural colours
```

---

## หลังได้รูปมาแล้ว

ส่งรูปกลับมาได้เลย จะจัดวางลงในหน้าผังการต่อสาย พร้อมใส่ลูกศรและป้ายกำกับภาษาไทยชี้ทีละจุดให้

และอย่าลืมกำกับไว้ใต้รูปที่สร้างด้วย AI ว่าเป็นภาพประกอบ ไม่ใช่ภาพอ้างอิงการต่อสาย เพื่อไม่ให้ครูคนอื่นที่เอาไปใช้ต่อเข้าใจผิด
