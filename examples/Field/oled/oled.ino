// Title: Oled
// Description: Bounces a character around the screen.
// Hardware: Daisy Field
// Author: Ben Sergentanis

#include "DaisyDuino.h"
#include <U8g2lib.h>

DaisyHardware hw;

// the magic incantation
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI
    oled(U8G2_R0, /* clock=*/8, /* data=*/10, /* cs=*/7, /* dc=*/9);

int x, y;
int xvel, yvel;
char str[] = "daisyfield";
int pos;

void setup() {
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  x = y = 30;
  xvel = yvel = -1;
  pos = 0;

  oled.setFont(u8g2_font_inb16_mf);
  oled.setFontDirection(0);
  oled.setFontMode(1);
  oled.begin();
}

void loop() {
  oled.clearBuffer();

  char c[] = {str[pos]};
  oled.drawStr(x, y, c);
  x += xvel;
  y += yvel;

  if (x <= 0 || x >= 110) {
    xvel *= -1;
    pos++;
    pos %= 10;
  }

  if (y <= 16 || y >= 64) {
    yvel *= -1;
    pos++;
    pos %= 10;
  }

  oled.sendBuffer();
  delay(5);
}