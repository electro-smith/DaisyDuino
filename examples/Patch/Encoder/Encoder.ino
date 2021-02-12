// Title: Encoder
// Description: Demonstrates usage of encoder and OLED
// Hardware: Daisy Patch
// Author: Ben Sergentanis

//  The paged mode contains all of the same features as the full buffer mode,
//  but runs much faster. It is still a *bit* slow, and is probably not
//  recommended over the leaner 8x8 mode 8x8 mode is much less feature rich, but
//  it's blazing fast, and is therefore recommended for use

#include "DaisyDuino.h"
#include <U8g2lib.h>
#include <U8x8lib.h>

DaisyHardware hw;

// Oleds
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI
    oled8x8(/* clock=*/8, /* data=*/10, /* cs=*/7, /* dc=*/9, /* reset=*/30);
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI oled(U8G2_R0, /* clock=*/8,
                                             /* data=*/10, /* cs=*/7, /* dc=*/9,
                                             /* reset=*/30);

int pos;

void setup() {
  hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);

  oled8x8.setFont(u8x8_font_chroma48medium8_r); /* 8x8 mode */
  oled.setFont(u8g2_font_inb16_mf);             /* Paged mode */

  oled8x8.begin();
  // oled.begin();
}

void EightByEight() {
  hw.DebounceControls();

  char *c = &((String)pos)[0];
  oled8x8.drawString(0, 0, c);

  pos += hw.encoder.Increment();
  pos = (pos % 10 + 10) % 10;

  if (hw.encoder.RisingEdge())
    pos = 0;
}

void Paged() {
  oled.firstPage();
  do {
    hw.DebounceControls();

    char *c = &((String)pos)[0];
    oled.drawStr(30, 30, c);

    pos += hw.encoder.Increment();
    pos = (pos % 10 + 10) % 10;

    if (hw.encoder.RisingEdge())
      pos = 0;
  } while (oled.nextPage());
}

void loop() {
  EightByEight(); /* 8x8 mode */
                  // Paged(); /* Paged mode */
}
