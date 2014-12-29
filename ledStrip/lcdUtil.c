#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>

#include "i2cUtil.h"
#include "lcdUtil.h"
#include "softKey.h"





int ttyfd;
struct termios options;
char *fnameTTY = "/dev/ttyAMA0";	/* Output for keyboard/mouse data */

void sendCmd(unsigned char cmd, unsigned char dataVal)
{
unsigned char serBuf[4];	/* Buffer for output data */
  serBuf[0] = cmd;
  serBuf[1] = dataVal;
  write(ttyfd, serBuf, 2);
}

void lcdInit(void)
{
unsigned char serBuf[4];	/* Buffer for output data */
int i;

  printf("lcdInit: Started\n");
  // Open UART port
  printf("lcdInit: Opening serial port\n");
  while ((ttyfd = open(fnameTTY, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
    fprintf(stderr, "Unable to open %s\n", fnameTTY);
    sleep(5);
  }
  //opening serial port causes one char to appear on serial port
  printf("lcdInit: Configuring serial port\n");
  tcgetattr(ttyfd, &options);
  cfmakeraw(&options);
  cfsetispeed(&options, USART_LCD_SPEED);
  cfsetospeed(&options, USART_LCD_SPEED);
  options.c_cflag &= ~PARENB;
  options.c_cflag |= CS8;
  tcsetattr(ttyfd, TCSAFLUSH, &options);
  printf("lcdInit: Serial port configured\n");

  // send some chars to clear LCD receive buffer
  serBuf[0] = 0x1;
  for (i=0;i<3;i++) {
    write(ttyfd, serBuf, 1);
  }
  // init display
  serBuf[0] = CMD_LCD_INIT;
  write(ttyfd, serBuf, 1);
  sleep(1); //need to wait for 9mS after lcd init

  // clear display
  //sendCmd(CMD_LCD, 0x01);
  //set backlight to red
  sendCmd(CMD_RED_BL, 0x00); 
  sendCmd(CMD_GREEN_BL, 0xff); 
  sendCmd(CMD_BLUE_BL, 0xff);

}


void lcdUpdate(char *lcdLine1, char *lcdLine2)
{
  int i;
  int line1Terminated = 0;
  int line2Terminated = 0;
  int line1Len = 0;
  int line2Len = 0;

  // replace non ASCII characters with space
  for (i=0;i< LCD_LINE_LEN; i++) { 
    if (lcdLine1[i] < 0x20 || lcdLine1[i] > 0x7d || line1Terminated == 1) { lcdLine1[i] = 0x20; line1Terminated = 1;} else line1Len++;
    if (lcdLine2[i] < 0x20 || lcdLine2[i] > 0x7d || line2Terminated == 1) { lcdLine2[i] = 0x20; line2Terminated = 1;} else line2Len++;
  }

  // clear display
  sendCmd(CMD_LCD, LCD_CLEAR_DISPLAY);
  //usleep(10);

  // rewrite the entire display
  sendCmd(CMD_LCD, LCD16X2_LINE1_START);
  write(ttyfd, lcdLine1, line1Len);
  sendCmd(CMD_LCD, LCD16X2_LINE2_START);
  write(ttyfd, lcdLine2, line2Len);

}

void setbackLight(unsigned char red, unsigned char green, unsigned char blue)
{
  sendCmd(CMD_RED_BL, red); 
  sendCmd(CMD_GREEN_BL, green); 
  sendCmd(CMD_BLUE_BL, blue);
}

unsigned char getButton()
{
unsigned char serBuf[5];	/* Buffer for input data */
  read(ttyfd, serBuf, 1);
  return serBuf[0];
}

void backLightRamp(unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char redStep, unsigned char greenStep, unsigned char blueStep)
{
static unsigned int redInc=1, greenInc=1, blueInc=1;

  if (redInc==1 && *red < (0xff-redStep) ) *red+=redStep; 
  else if (*red > redStep) {redInc=0; *red-=redStep;}
  else redInc=1;
  if (greenInc==1 && *green < (0xff - greenStep) ) *green+=greenStep; 
  else if (*green > greenStep) {greenInc=0; *green-=greenStep;}
  else greenInc=1;
  if (blueInc==1 && *blue < (0xff - blueStep) ) *blue+=blueStep; 
  else if (*blue > blueStep) {blueInc=0; *blue-=blueStep;}
  else blueInc=1;
}

// ---------- random colors
void backLightFlash(unsigned char *red, unsigned char *green, unsigned char *blue)
{
static unsigned int delayCnt=0;
  if (delayCnt == 10) {
    *red=0xff & rand(); 
    *green=0xff & rand(); 
    *blue = 0xff & rand();
    delayCnt = 0;
  }
  else delayCnt++;
}

// ------------- Alternate color, dark...
void backLightFlash2(unsigned char *red, unsigned char *green, unsigned char *blue)
{
static unsigned int delayCnt=0;
static unsigned char rgbState=1;
  if (delayCnt == 5) {
    *red=0xff; 
    *green=0xff; 
    *blue =0xff;
    switch (rgbState) {
      case 1: *red=0x0; break;
      case 3: *green=0x0; break;
      case 5: *blue=0x0; break;
      case 7: *red=0x0; *green=0x0; break;
      case 9: *red=0x0; *blue=0x0; break;
      case 11: *green=0x0; *blue=0x0; break;
      case 13: *red=0x0; *green=0x0; *blue=0x0; break;
    }
    delayCnt = 0;
    if (rgbState == 14) rgbState = 1;
    else rgbState += 1;
  }
  else delayCnt++;
}

void backLightTemp(float temperature, unsigned char *red, unsigned char *green, unsigned char *blue)
{
float myTemp;

  myTemp = (int) temperature;
  if (myTemp < MIN_TEMP_BL) myTemp = MIN_TEMP_BL;
  if (myTemp > MAX_TEMP_BL) myTemp = MAX_TEMP_BL;
  myTemp = myTemp - MIN_TEMP_BL;
  *blue = (unsigned char) (SCALE_TEMP_BL * myTemp);
  *red = 0xff - *blue;
  *green = 0xff;
}

void ledTempState (int tempCtrlMode, int heatActive, int coolActive)
{
  switch(tempCtrlMode) {
    case HEAT_CTRL_MODE:
      if (heatActive == 1)
        sendCmd(CMD_BI_COL_LED, RED_LED_BLINK | GREEN_LED_DARK);
      else
        sendCmd(CMD_BI_COL_LED, RED_LED_STEADY | GREEN_LED_DARK);
      break;
    case COOL_CTRL_MODE:
      if (coolActive == 1)
        sendCmd(CMD_BI_COL_LED, RED_LED_DARK | GREEN_LED_BLINK);
      else
        sendCmd(CMD_BI_COL_LED, RED_LED_DARK | GREEN_LED_STEADY);
      break;
    default:
      sendCmd(CMD_BI_COL_LED, RED_LED_DARK | GREEN_LED_DARK);
      break;
  }
}
