// -------------------- lpd8806.c -------------------------
#include "gpio.h"
#include "lpd8806.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void write_latch(unsigned int len) {
  unsigned int txData[3];

  // Latch length varies with the number of LEDs:
  // Only one byte required to latch final Blue byte, but require one extra byte per 32 leds
  len = (len + 31) / 32;
  txData[0] = 0x0;
  len=2;

  while(len>0) {
    putsSPI2(1, txData);
    len--;
  }
}

void send_frame(unsigned long int led_strip_colors[], unsigned int ledStripLen, unsigned int litLen) {
  unsigned int i,j;
  unsigned int red,green,blue;
  unsigned int txData[3];

  j=0;
  for(i=0;i<ledStripLen;i++) {
    blue = (unsigned int) (led_strip_colors[j] & 0xFF);
    green = (unsigned int) ((led_strip_colors[j] >> 8) & 0xFF);
    red = (unsigned int) ((led_strip_colors[j] >> 16) & 0xFF);
    if (i < litLen) {
      txData[0] = green|0x80;
      txData[1] = red|0x80;
      txData[2] = blue|0x80;
    }
    else {
      txData[0] = 0x80;
      txData[1] = 0x80;
      txData[2] = 0x80;
    }
    putsSPI2(3, txData);
    if (j==2) j=0; else j++;
  }
  write_latch(ledStripLen);
}


void send_frame_oneColor(unsigned int red, unsigned int green, unsigned int blue, unsigned int ledStripLen, unsigned int litLen) {
  unsigned int i;
  unsigned int txData[3];

  txData[0] = green|0x80;
  txData[1] = red|0x80;
  txData[2] = blue|0x80;
  for(i=0;i<ledStripLen;i++) {
    if (i >= litLen) {
      txData[0] = 0x80;
      txData[1] = 0x80;
      txData[2] = 0x80;
    }
    putsSPI2(3, txData);
  }
  write_latch(ledStripLen);
}


void blendPixels(unsigned long int led_strip_colors1[], unsigned long int led_strip_colors2[], unsigned int stepNum, unsigned int delayVal) {
int i;
float stepVal[9];
float pixVal[9];
float pix1, pix2;
unsigned long int led_strip_colors[3];
int j;
int stepCnt;

  //calculate step values and step directions
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      pix1 = (float) ((led_strip_colors1[i] >> (8*j)) & 0xff);
      pixVal[i*3+j] = pix1;
      pix2 = (float) ((led_strip_colors2[i] >> (8*j)) & 0xff);
      stepVal[i*3+j] = (pix2 - pix1) / (float) stepNum;
    }
  }

  //slowly migrate pixels from start to finish
  for (stepCnt=0; stepCnt<stepNum; stepCnt++) {
    //recombine the pixels
    for (i=0;i<3;i++) {
      led_strip_colors[i] = ((unsigned long int) pixVal[i*3+2] << 16) |  ( (unsigned long int) pixVal[i*3+1] << 8) |  ( (unsigned long int) pixVal[i*3]);
      //if (i == 2) printf("led_strip_colors[%d] = 0x%06lx\n", i, led_strip_colors[i]);
    }
    //update the pixels
    for (i=0;i<9;i++) {
      pixVal[i] = pixVal[i] + stepVal[i];
      if (pixVal[i] > 127.0f) pixVal[i] = 127.0f;
      if (pixVal[i] < 0.0f) pixVal[i] = 0.0f;
    }
    //update the led strip
    send_frame(led_strip_colors, LED_STRIP_LEN, LED_STRIP_LEN);
    usleep(delayVal);
  }


}


