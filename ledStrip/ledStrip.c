// ----------------------------------------------------------------
// -------------------- ledStrip --------------------------------
// ----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "ledStrip.h"
#include "gpio.h"
#include "lpd8806.h"

unsigned long int led_strip_colors[3];
unsigned long int led_strip_colors1[3];
unsigned long int led_strip_colors2[3];
unsigned long int led_strip_colors3[3];
unsigned long int led_strip_colors4[3];
unsigned long int led_strip_colors5[3];
unsigned long int led_strip_colors6[3];
unsigned long int led_strip_colors7[3];
unsigned long int led_strip_colors8[3];
void usage(char *s);
#define COL_DISP_DEL 350

int main(int argc, char **argv)
{
unsigned int litLen;
unsigned int passNum=0;
int ledInc;

  printf("ledStrip: Started\n");


  // configure GPIO
  printf("ledStrip: Configuring GPIO\n");
  gpioInit();
  litLen = 0;

  led_strip_colors1[0] = 0x7F0000;
  led_strip_colors1[1] = 0x007f00;
  led_strip_colors1[2] = 0x00007f;

  led_strip_colors2[0] = 0x7f7f7f;
  led_strip_colors2[1] = 0x7f7f7f;
  led_strip_colors2[2] = 0x7f7f7f;

  led_strip_colors3[0] = 0x7f1000;
  led_strip_colors3[1] = 0x007f00;
  led_strip_colors3[2] = 0x10007f;

  led_strip_colors4[0] = 0x7f0000;
  led_strip_colors4[1] = 0x7f0000;
  led_strip_colors4[2] = 0x7f0000;

  led_strip_colors5[0] = 0x7f0000;
  led_strip_colors5[1] = 0x007f00;
  led_strip_colors5[2] = 0x00007f;

  led_strip_colors6[0] = 0x007f00;
  led_strip_colors6[1] = 0x007f00;
  led_strip_colors6[2] = 0x007f00;

  led_strip_colors7[0] = 0x7f1000;
  led_strip_colors7[1] = 0x7f1000;
  led_strip_colors7[2] = 0x7f1000;

  led_strip_colors8[0] = 0x7f7f7f;
  led_strip_colors8[1] = 0x7f7f7f;
  led_strip_colors8[2] = 0x7f7f7f;

  while (1) {
    blendPixels(led_strip_colors1, led_strip_colors2, 100, 200000);
    blendPixels(led_strip_colors2, led_strip_colors3, 100, 200000);
    blendPixels(led_strip_colors3, led_strip_colors4, 100, 200000);
    blendPixels(led_strip_colors4, led_strip_colors5, 100, 200000);
    blendPixels(led_strip_colors5, led_strip_colors6, 100, 200000);
    blendPixels(led_strip_colors6, led_strip_colors7, 100, 200000);
    blendPixels(led_strip_colors7, led_strip_colors1, 100, 200000);
  }

  while (1) {
    if (passNum == COL_DISP_DEL*7) passNum = 0;
    else passNum++;
    if (litLen == LED_STRIP_LEN)
      ledInc = -1;
    if (litLen == 0)
      ledInc = 1;
    litLen+=ledInc;
    if (passNum < COL_DISP_DEL) {
      led_strip_colors[0] = 0x7F0000;
      led_strip_colors[1] = 0x007f00;
      led_strip_colors[2] = 0x00007f;
    }
    else if (passNum < COL_DISP_DEL*2) {
      led_strip_colors[0] = 0x7f0010;
      led_strip_colors[1] = 0x7f7f00;
      led_strip_colors[2] = 0x10007f;
    }
    else if (passNum < COL_DISP_DEL*3) {
      led_strip_colors[0] = 0x7f1000;
      led_strip_colors[1] = 0x007f00;
      led_strip_colors[2] = 0x10007f;
    }
    else if (passNum < COL_DISP_DEL*4) {
      led_strip_colors[0] = 0x7f1000;
      led_strip_colors[1] = 0x7f1000;
      led_strip_colors[2] = 0x7f1000;
    }
    else if (passNum < COL_DISP_DEL*5) {
      led_strip_colors[0] = 0x7f0000;
      led_strip_colors[1] = 0x007f00;
      led_strip_colors[2] = 0x00007f;
    }
    else if (passNum < COL_DISP_DEL*6) {
      led_strip_colors[0] = 0x64003d;
      led_strip_colors[1] = 0x7f1000;
      led_strip_colors[2] = 0x1d0a57;
    }
    else {
      led_strip_colors[0] = 0x7f1000;
      led_strip_colors[1] = 0x7f1000;
      led_strip_colors[2] = 0x7f1000;
    }
    send_frame(led_strip_colors, LED_STRIP_LEN, litLen);
    usleep(100000);
  }


}



