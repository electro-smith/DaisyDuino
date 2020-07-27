#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "DaisyDuino.h"

DaisyHardware hw;

//the magic incantation
U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 8, /* data=*/ 10, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 30);

int x, y;
int xvel, yvel;
char str[] = "daisypatch";
int pos;

void setup()
{
    hw = DAISY.init(DAISY_PATCH, AUDIO_SR_48K);
    x = y = 30;
    xvel = yvel = -1;
    pos = 0;

    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.setFontDirection(0);
    u8g2.setFontMode(1);
    u8g2.begin();
}

void loop()
{
    u8g2.clearBuffer();

    char c[] = {str[pos]};
    u8g2.drawStr(x, y, c);
    x += xvel;
    y += yvel;

    if (x <= 0 || x >= 110){
        xvel *= -1;
        pos++;
        pos %= 10;
    }
    
    if (y <= 16 || y >= 64){
        yvel *= -1;
        pos++;
        pos %= 10;
    }

    u8g2.sendBuffer();
    delay(5);
} 



// Title: Oled
// Description: 
// Hardware: Daisy Patch
// Author: Ben Sergentanis
// Controls: 
// Button 1: Trigger next note
// Knob 1: Decay time
// Knob 2: Reverb amount
// Diagram: https://raw.githubusercontent.com/electro-smith/DaisyExamples/master/pod/MusicBox/resources/MusicBox.png
