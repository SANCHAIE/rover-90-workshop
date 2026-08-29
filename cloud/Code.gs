/* =====================================================================
   Rover Workshop · Google Apps Script
   ---------------------------------------------------------------------
   รับข้อมูลจาก ESP32 แล้วเขียนลงชีตชื่อ "data"
   ติดตั้งครั้งเดียว ใช้ได้ทุกปี  ดูขั้นตอนใน 00_ติดตั้ง_GoogleSheet.md
   ===================================================================== */

const SHEET_NAME = 'data';

const HEADERS = [
  'เวลา',
  'ทีม',
  'ms ต่อ cm',
  'TRIM ซ้าย',
  'TRIM ขวา',
  'คลาดเคลื่อนรอบ 1 (ซม.)',
  'คลาดเคลื่อนรอบ 2 (ซม.)',
  'คลาดเคลื่อนเฉลี่ย (ซม.)',
  'ระยะที่เซนเซอร์อ่านได้ (ซม.)'
];

/* ---------------------------------------------------------------------
   ฟังก์ชันหลัก - ESP32 เรียกมาที่นี่
   --------------------------------------------------------------------- */
function doGet(e) {
  const lock = LockService.getScriptLock();

  // กัน 4 ทีมส่งพร้อมกันแล้วข้อมูลทับกัน
  try {
    lock.waitLock(20000);
  } catch (err) {
    return ContentService.createTextOutput('BUSY');
  }

  try {
    const sheet = getSheet_();
    const p = (e && e.parameter) ? e.parameter : {};

    const team = toNum_(p.team);
    const err1 = toNum_(p.err1);
    const err2 = toNum_(p.err2);

    // คลาดเคลื่อนเฉลี่ย ใช้ค่าสัมบูรณ์ เพราะเกินเป้ากับขาดเป้าแย่พอกัน
    let avg = '';
    if (err1 !== '' && err2 !== '') {
      avg = Math.round((Math.abs(err1) + Math.abs(err2)) / 2 * 100) / 100;
    } else if (err1 !== '') {
      avg = Math.abs(err1);
    }

    sheet.appendRow([
      new Date(),
      team,
      toNum_(p.mspercm),
      toNum_(p.trimL),
      toNum_(p.trimR),
      err1,
      err2,
      avg,
      toNum_(p.dist)
    ]);

    return ContentService.createTextOutput('OK team ' + team);

  } catch (err) {
    return ContentService.createTextOutput('ERROR ' + err);
  } finally {
    lock.releaseLock();
  }
}

/* ---------------------------------------------------------------------
   ฟังก์ชันช่วย
   --------------------------------------------------------------------- */
function getSheet_() {
  const ss = SpreadsheetApp.getActiveSpreadsheet();
  let sheet = ss.getSheetByName(SHEET_NAME);

  if (!sheet) {
    sheet = ss.insertSheet(SHEET_NAME);
  }
  if (sheet.getLastRow() === 0) {
    sheet.appendRow(HEADERS);
    sheet.setFrozenRows(1);
    sheet.getRange(1, 1, 1, HEADERS.length).setFontWeight('bold');
    sheet.setColumnWidth(1, 150);
  }
  return sheet;
}

function toNum_(v) {
  if (v === undefined || v === null || v === '') return '';
  const n = parseFloat(v);
  return isNaN(n) ? '' : n;
}

/* ---------------------------------------------------------------------
   สองฟังก์ชันนี้สำหรับครูกดเองในหน้า Apps Script
   --------------------------------------------------------------------- */

// กดปุ่ม Run ที่ฟังก์ชันนี้ เพื่อทดสอบว่าเขียนชีตได้จริง
// ต้องกดครั้งแรกเพื่อขออนุญาต (Authorize) ก่อน Deploy ด้วย
function testAddRow() {          // ทดสอบเพิ่มข้อมูล
  doGet({
    parameter: {
      team: '99', mspercm: '22.5', trimL: '0.95', trimR: '1.00',
      err1: '2.4', err2: '-1.1', dist: '31.5'
    }
  });
  Logger.log('เพิ่มแถวทดสอบของทีม 99 แล้ว');
}

// ล้างข้อมูลเก่าก่อนเริ่มคาบใหม่ หัวตารางยังอยู่
function clearBeforeClass() {    // ล้างข้อมูลก่อนเริ่มคาบ
  const sheet = getSheet_();
  const last = sheet.getLastRow();
  if (last > 1) {
    sheet.deleteRows(2, last - 1);
  }
  Logger.log('ล้างข้อมูลเรียบร้อย พร้อมเริ่มคาบใหม่');
}
