#include "../../options.h"
#if DSP_MODEL==5

#include "displaySH1106.h"
#include <Wire.h>
#include "../../player.h"
#include "../../config.h"
#include "../../network.h"

#ifndef SCREEN_ADDRESS
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32 or scan it https://create.arduino.cc/projecthub/abdularbi17/how-to-scan-i2c-address-in-arduino-eaadda
#endif

#define LOGO_WIDTH 21
#define LOGO_HEIGHT 32

const char *dow[7] = {"вс","пн","вт","ср","чт","пт","сб"};

const unsigned char logo [] PROGMEM=
{
    0x06, 0x03, 0x00, 0x0f, 0x07, 0x80, 0x1f, 0x8f, 0xc0, 0x1f, 0x8f, 0xc0,
    0x0f, 0x07, 0x80, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x03, 0xff, 0x00, 0x0f, 0xff, 0x80,
    0x1f, 0xff, 0xc0, 0x1f, 0xff, 0xc0, 0x3f, 0x8f, 0xe0, 0x7e, 0x03, 0xf0,
    0x7c, 0x01, 0xf0, 0x7c, 0x01, 0xf0, 0x7f, 0xff, 0xf0, 0xff, 0xff, 0xf8,
    0xff, 0xff, 0xf8, 0xff, 0xff, 0xf8, 0x7c, 0x00, 0x00, 0x7c, 0x00, 0x00,
    0x7e, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x3f, 0xc0, 0xe0, 0x3f, 0xff, 0xe0,
    0x1f, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xfe, 0x00
};

TwoWire I2CSH1106 = TwoWire(0);

DisplaySH1106::DisplaySH1106(): Adafruit_SH1106G(128, 64, &I2CSH1106, -1) {

}

char* DisplaySH1106::utf8Rus(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  bool E = false;
  strlcpy(strn, str, BUFLEN);
  if (uppercase) {
    bool next = false;
    for (char *iter = strn; *iter != '\0'; ++iter)
    {
      if (E) {
        E = false;
        continue;
      }
      byte rus = (byte) * iter;
      if (rus == 208 && (byte) * (iter + 1) == 129) { // ёКостыли
        *iter = (char)209;
        *(iter + 1) = (char)145;
        E = true;
        continue;
      }
      if (rus == 209 && (byte) * (iter + 1) == 145) {
        *iter = (char)209;
        *(iter + 1) = (char)145;
        E = true;
        continue;
      }
      if (next) {
        if (rus >= 128 && rus <= 143) *iter = (char)(rus + 32);
        if (rus >= 176 && rus <= 191) *iter = (char)(rus - 32);
        next = false;
      }
      if (rus == 208) next = true;
      if (rus == 209) {
        *iter = (char)208;
        next = true;
      }
      *iter = toupper(*iter);
    }
  }
  while (strn[index])
  {
    if (strn[index] >= 0xBF)
    {
      switch (strn[index]) {
        case 0xD0: {
            if (strn[index + 1] == 0x81) {
              strn[index] = 0xA8;
              break;
            }
            if (strn[index + 1] >= 0x90 && strn[index + 1] <= 0xBF) strn[index] = strn[index + 1] + 0x30;
            break;
          }
        case 0xD1: {
            if (strn[index + 1] == 0x91) {
              //strn[index] = 0xB7;
              strn[index] = 0xB8;
              break;
            }
            if (strn[index + 1] >= 0x80 && strn[index + 1] <= 0x8F) strn[index] = strn[index + 1] + 0x70;
            break;
          }
      }
      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }
    index++;
  }
  return strn;
}

void DisplaySH1106::apScreen() {
  setTextSize(1);
  setTextColor(TFT_FG, TFT_BG);
  setCursor(TFT_FRAMEWDT, TFT_FRAMEWDT + 2 * TFT_LINEHGHT);
  print("AP NAME: ");
  print(apSsid);
  setCursor(TFT_FRAMEWDT, TFT_FRAMEWDT + 3 * TFT_LINEHGHT);
  print("PASSWORD: ");
  print(apPassword);
  setTextColor(SILVER, TFT_BG);
  setCursor(TFT_FRAMEWDT, sheight - TFT_LINEHGHT * 2);
  print("SETTINGS PAGE ON: ");
  setCursor(TFT_FRAMEWDT, sheight - TFT_LINEHGHT);
  print("http://");
  print(WiFi.softAPIP().toString().c_str());
  print("/");
}

void DisplaySH1106::initD(uint16_t &screenwidth, uint16_t &screenheight) {
  I2CSH1106.begin(I2C_SDA, I2C_SCL, (uint32_t)100000);
  if (!begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("SH1106 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  cp437(true);
  fillScreen(TFT_BG);
  setRotation(TFT_ROTATE);
  setTextWrap(false);
  screenwidth = width();
  screenheight = height();
  swidth = screenwidth;
  sheight = screenheight;
}

void DisplaySH1106::drawLogo() {
  clearDisplay();
  drawBitmap(
    (width()  - LOGO_WIDTH ) / 2,
    8,
    logo, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display();
}

void DisplaySH1106::drawPlaylist(uint16_t currentItem, char* currentItemText) {
  for (byte i = 0; i < PLMITEMS; i++) {
    plMenu[i][0] = '\0';
  }
  config.fillPlMenu(plMenu, currentItem - 3, PLMITEMS);
  setTextSize(1);
  int yStart = (sheight / 2 - PLMITEMHEIGHT / 2) - PLMITEMHEIGHT * (PLMITEMS - 1) / 2 + 3;
  fillRect(0, (sheight / 2 - PLMITEMHEIGHT / 2) + 1, swidth, PLMITEMHEIGHT, TFT_LOGO);
  setTextColor(TFT_FG, TFT_BG);
  for (byte i = 0; i < PLMITEMS; i++) {
    if (i == 3) {
      strlcpy(currentItemText, plMenu[i], PLMITEMLENGHT - 1);
    } else {
      setCursor(TFT_FRAMEWDT, yStart + i * PLMITEMHEIGHT);
      print(utf8Rus(plMenu[i], true));
    }
  }
}

void DisplaySH1106::clearDsp() {
  fillScreen(TFT_BG);
}

void DisplaySH1106::drawScrollFrame(uint16_t texttop, uint16_t textheight, uint16_t bg) {
  if (TFT_FRAMEWDT == 0) return;
  fillRect(0, texttop, TFT_FRAMEWDT, textheight, bg);
  fillRect(swidth - TFT_FRAMEWDT, texttop, TFT_FRAMEWDT, textheight, bg);
}

void DisplaySH1106::getScrolBbounds(const char* text, const char* separator, byte textsize, uint16_t &tWidth, uint16_t &tHeight, uint16_t &sWidth) {
  int16_t  x1, y1;
  uint16_t w, h;
  setTextSize(textsize);
  getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tWidth = w;
  tHeight = h;
  getTextBounds(separator, 0, 0, &x1, &y1, &w, &h);
  sWidth = w;
}

void DisplaySH1106::clearScroll(uint16_t texttop, uint16_t textheight, uint16_t bg) {
  fillRect(0,  texttop, swidth, textheight, bg);
}

void DisplaySH1106::centerText(const char* text, byte y, uint16_t fg, uint16_t bg) {
  int16_t  x1, y1;
  uint16_t w, h;
  const char* txt = text;
  getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
  setTextColor(fg,bg);
  if(y==90) y=sheight-TFT_LINEHGHT*2-5;
  if(y==110) y=sheight-TFT_LINEHGHT;
  setCursor((swidth - w) / 2, y);
  fillRect(0, y, swidth, h, bg);
  print(txt);
}

void DisplaySH1106::rightText(const char* text, byte y, uint16_t fg, uint16_t bg) {
  int16_t  x1, y1;
  uint16_t w, h;
  getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  setTextColor(fg,bg);
  setCursor(swidth - w - TFT_FRAMEWDT, y);
  fillRect(swidth - w - TFT_FRAMEWDT, y, w, h, bg);
  print(text);
}

void DisplaySH1106::displayHeapForDebug() {

}

void DisplaySH1106::printClock(const char* timestr) {
  setTextSize(2);
  centerText(timestr, 34, TFT_FG, TFT_BG);
  setTextSize(1);
}

#define CLCLF 34

void DisplaySH1106::printClock(struct tm timeinfo, bool dots, bool redraw) {
  char timeStringBuff[20] = { 0 };
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
  setTextSize(2);
  setCursor(CLCLF, 34);
  setTextColor(TFT_FG, TFT_BG);
  print(timeStringBuff);
  setTextSize(1);
  setCursor(CLCLF + 6*2*5+1, 34+1);
  sprintf(timeStringBuff, "%02d", timeinfo.tm_sec);
  print(timeStringBuff);
}

void DisplaySH1106::drawVolumeBar(bool withNumber) {
  int16_t vTop = sheight - 4;
  int16_t vWidth = swidth;
  uint8_t ww = map(config.store.volume, 0, 254, 0, vWidth - 2);
  fillRect(TFT_FRAMEWDT, vTop, vWidth, 3, TFT_BG);
  drawRect(TFT_FRAMEWDT, vTop, vWidth, 3, TFT_LOGO);
  fillRect(TFT_FRAMEWDT + 1, vTop + 1, ww, 1, TFT_LOGO);
  if (withNumber) {
    setTextSize(2);
    setTextColor(TFT_FG);
    char volstr[4];
    uint16_t wv, hv;
    int16_t  x1, y1;
    sprintf(volstr, "%d", config.store.volume);
    getTextBounds(volstr, 0, 0, &x1, &y1, &wv, &hv);
    fillRect(TFT_FRAMEWDT, 24, swidth - TFT_FRAMEWDT / 2, hv + 3, TFT_BG);
    setCursor((swidth - wv) / 2, 24);
    print(volstr);
  }
}

void DisplaySH1106::drawNextStationNum(uint16_t num) {
  setTextSize(2);
  setTextColor(TFT_FG);
  char numstr[7];
  uint16_t wv, hv;
  int16_t  x1, y1;
  sprintf(numstr, "%d", num);
  getTextBounds(numstr, 0, 0, &x1, &y1, &wv, &hv);
  fillRect(TFT_FRAMEWDT, 24, swidth - TFT_FRAMEWDT / 2, hv + 3, TFT_BG);
  setCursor((swidth - wv) / 2, 24);
  print(numstr);
}

void DisplaySH1106::frameTitle(const char* str) {
  setTextSize(2);
  centerText(str, TFT_FRAMEWDT, TFT_LOGO, TFT_BG);
}

void DisplaySH1106::rssi(const char* str) {
  char buf[4];
  strlcpy(buf, str, strlen(str)-2);
  int16_t vTop = sheight - TFT_LINEHGHT - 4;
  setTextSize(1);
  rightText(buf, vTop, SILVER, TFT_BG);
}

void DisplaySH1106::ip(const char* str) {
  int16_t vTop = sheight - TFT_LINEHGHT - 4;
  setTextSize(1);
  setTextColor(SILVER, TFT_BG);
  setCursor(0, vTop);
  print(str);
}

void DisplaySH1106::set_TextSize(uint8_t s) {
  setTextSize(s);
}

void DisplaySH1106::set_TextColor(uint16_t fg, uint16_t bg) {
  setTextColor(fg, bg);
}

void DisplaySH1106::set_Cursor(int16_t x, int16_t y) {
  setCursor(x, y);
}

void DisplaySH1106::printText(const char* txt) {
  print(txt);
}

void DisplaySH1106::loop() {
  if (checkdelay(SCROLLTIME, loopdelay)) {
    display();
  }
  yield();
}

boolean DisplaySH1106::checkdelay(int m, unsigned long &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

#endif