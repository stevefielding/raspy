// ----------------------------------------------------------------
// -------------------- wiFiThermo --------------------------------
// ----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "lcdUtil.h"
#include "wiFiThermo.h"
#include "i2cUtil.h"
#include "softKey.h"
#include "gpio.h"
#include "logData.h"
#include <time.h>

void usage(char *s);

int main(int argc, char **argv)
{
  printf("wiFiThermo: Started\n");
  int i;
  char lcdLine1[LCD_LINE_LEN+1];	/* Buffer for lcd data */
  char lcdLine2[LCD_LINE_LEN+1];	/* Buffer for lcd data */
  unsigned int passNum = 0;
  unsigned char red, green, blue;
  int i2c1_fd;
  unsigned int current_sint;
  float voltage_sint;
  unsigned int current_s1;
  float voltage_s1;
  unsigned int current_s2;
  float voltage_s2;
  unsigned int current_s3;
  float voltage_s3;
  float temperature;
  struct sysState mySysState;
  unsigned char keyp;
  char softKeyText[LCD_LINE_LEN+1];
  int mySoftKeysActive;
  int heatActive=0;
  int coolActive=0;
  int teethMusicPlayed = 0;
  int tabletMusicPlayed = 0;
  int bandsMusicPlayed = 0;
  int stretchMusicPlayed = 0;
  long int runTimeSec = 0;
  long int runTimeMin = 0;
  long int runTimeHour = 0;
  long int dayRunTime = 0;

  // configure GPIO
  printf("wiFiThermo: Configuring GPIO\n");
  gpioInit();

  lcdInit();

  // init i2c
  printf("--- Initializing I2C sub-system\n");
  if ((i2c1_fd = open ("/dev/i2c-1", O_RDWR)) == -1) { 
    printf ("i2c1_fd: Unable to initialise I2C-1\n") ;
    return 1 ;
  }

  ina226Init(i2c1_fd, INA226_SUPPLY_INT_I2C_SLAVE_ADDR);
  ina226Init(i2c1_fd, INA226_SUPPLY1_I2C_SLAVE_ADDR);
  ina226Init(i2c1_fd, INA226_SUPPLY2_I2C_SLAVE_ADDR);
  ina226Init(i2c1_fd, INA226_SUPPLY3_I2C_SLAVE_ADDR);
  ad7410Init(i2c1_fd, ADT7410_I2C_SLAVE_ADDR);
  printf("wiFiThermo: Starting test\n");

  for (i=0;i<20;i++) {
    lcdLine1[i] = 0x20;
    lcdLine2[i] = 0x20;
  }

  mySysState.tempSetPoint = 70.0f;
  mySysState.tempCtrlMode = OFF_CTRL_MODE;
  mySysState.tempFormat = FAHRENHEIT_DISP_MODE;
  mySysState.message = OFF_MESSAGE;
  mySysState.music = NO_MUSIC;
  mySysState.teeth = ALERT_OFF;
  mySysState.tablet = ALERT_OFF;
  mySysState.bands = ALERT_OFF;
  mySysState.stretch = ALERT_OFF;
  
	//FILE *fp = popen("aplay /home/pi/audio/police_s.wav &", "r");
  popen("omxplayer /home/pi/audio/menu/music1.mp3 &", "r");
  while (1) {
time_t t = time(NULL);
struct tm tm = *localtime(&t);

printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    passNum++;
    keyp = getButton();
    mySoftKeysActive = softKeyUpdate(softKeyText, keyp, &mySysState);

    //if soft keys menu not active, then process state
    if (mySoftKeysActive == 0) {

      //enable teeth alert at 10am
      if ( tm.tm_hour == 10 && tm.tm_min == 0) {
        mySysState.teeth = ALERT_ON;
      }
      //enable tablet alert at 12pm
      //if ( tm.tm_hour == 12 && tm.tm_min == 0) {
      //  mySysState.tablet = ALERT_ON;
      //}
      //enable bands alert at 12:10
      //if ( tm.tm_hour == 12 && tm.tm_min == 10) {
      //  mySysState.bands = ALERT_ON;
      //}
      //enable stretch alert at 16:40
      //if ( tm.tm_hour == 16 && tm.tm_min == 40) {
      //  mySysState.stretch = ALERT_ON;
      //}

      //--- process alerts

      //teeth alert
      if (mySysState.teeth == ALERT_ON) {
        if ( tm.tm_hour >= 10 && (tm.tm_min == 0 || tm.tm_min == 15 || tm.tm_min == 30 || tm.tm_min == 45)  ) {
          if (teethMusicPlayed == 0) {
            popen("omxplayer /home/pi/audio/menu/teeth.mp3 &", "r");
            //popen("aplay /home/pi/audio/police_s.wav &", "r");
            teethMusicPlayed = 1;
	  }
        }
	else
          teethMusicPlayed = 0;
      }

      //tablet alert
      if (mySysState.tablet == ALERT_ON) {
        if ( tm.tm_hour >= 12 && tm.tm_min == 7 ) {
          if (tabletMusicPlayed == 0) {
            popen("omxplayer /home/pi/audio/menu/tablet.mp3 &", "r");
            tabletMusicPlayed = 1;
	  }
        }
	else
          tabletMusicPlayed = 0;
      }

      //bands alert
      if (mySysState.bands == ALERT_ON) {
        if ( tm.tm_hour >= 12 && tm.tm_min == 20 ) {
          if (bandsMusicPlayed == 0) {
            popen("omxplayer /home/pi/audio/menu/bands.mp3 &", "r");
            bandsMusicPlayed = 1;
	  }
        }
	else
          bandsMusicPlayed = 0;
      }

      //stretch alert
      if (mySysState.stretch == ALERT_ON) {
        if ( tm.tm_hour >= 16 && tm.tm_min == 50 ) {
          if (stretchMusicPlayed == 0) {
            popen("omxplayer /home/pi/audio/menu/stretch.mp3 &", "r");
            stretchMusicPlayed = 1;
	  }
        }
	else
          stretchMusicPlayed = 0;
      }

      //process music requests
      if (mySysState.music != NO_MUSIC) {
	switch (mySysState.music) {
          case MUSIC_1: popen("omxplayer /home/pi/audio/menu/music1.mp3 &", "r"); break;
          case MUSIC_2: popen("omxplayer /home/pi/audio/menu/music2.mp3 &", "r"); break;
          case MUSIC_3: popen("omxplayer /home/pi/audio/menu/music3.mp3 &", "r"); break;
          case MUSIC_4: popen("omxplayer /home/pi/audio/menu/music4.mp3 &", "r"); break;
        }
        mySysState.music = NO_MUSIC;
	//popen("omxplayer /home/pi/audio/fantastic80s/01\\ Young\\ Guns.mp3 &", "r");
      }

      //if not message state, then update heating system status display
      if (mySysState.message == OFF_MESSAGE) {
        backLightTemp(temperature * 1.8f + 32.0f, &red, &green, &blue);
        switch (mySysState.tempCtrlMode) {
          case OFF_CTRL_MODE: sprintf(lcdLine2,   "------ Off -----"); break;
          case HEAT_CTRL_MODE: sprintf(lcdLine2,  "----- Heat -----"); break;
          case COOL_CTRL_MODE: sprintf(lcdLine2,  "------ AC ------"); break;
          case FAN_CTRL_MODE: sprintf(lcdLine2,   "------ Fan -----"); break;
          default: sprintf(lcdLine2,              "----- Error ----"); break;
        }
      }
      //else process message state
      else {
        switch(mySysState.message) {
          case (READ_MESSAGE): strcpy(lcdLine2,     "Read Msg Below "); backLightRamp(&red, &green, &blue, 0x4, 0x8, 0xc); break;
          case (BIRTHDAY_MESSAGE): strcpy(lcdLine2, "Happy Birthday "); backLightFlash(&red, &green, &blue); break;
          default: strcpy(lcdLine2,                 "---- Error ---- "); red=0; green=0xff; blue=0xff; break;
        }
      }
    }
    else {
      strcpy(lcdLine2, softKeyText);
      red=0; green=0; blue=0;
    }

    setbackLight(red, green, blue);

    //Get current, voltage and temperature
    ina226GetMeas(i2c1_fd, INA226_SUPPLY_INT_I2C_SLAVE_ADDR, "Supply Internal", &voltage_sint, &current_sint);
    ina226GetMeas(i2c1_fd, INA226_SUPPLY1_I2C_SLAVE_ADDR, "Supply1", &voltage_s1, &current_s1);
    ina226GetMeas(i2c1_fd, INA226_SUPPLY2_I2C_SLAVE_ADDR, "Supply2", &voltage_s2, &current_s2);
    ina226GetMeas(i2c1_fd, INA226_SUPPLY3_I2C_SLAVE_ADDR, "Supply3", &voltage_s3, &current_s3);
    temperature = ad7410GetTemp(i2c1_fd, ADT7410_I2C_SLAVE_ADDR);

    // update tempCtrl state and led state after soft key input goes inactive. Update run time log
    if (mySoftKeysActive == 0) {
      tempCtrl(mySysState.tempCtrlMode, mySysState.tempSetPoint, temperature * 1.8f + 32.0f, &heatActive, &coolActive);
      ledTempState (mySysState.tempCtrlMode, heatActive, coolActive);
      logRunTime (heatActive, coolActive, &dayRunTime);
    }

    runTimeSec = dayRunTime % 60;
    runTimeMin = ((dayRunTime - runTimeSec) / 60) % 60;
    runTimeHour = (((dayRunTime - runTimeSec) / 60) - runTimeMin) / 60;

    printf("Temp = %.2f C\n", temperature);
    //sprintf(lcdLine1, "%.0fC %.1fF %02d:%02d\n", temperature, temperature * 1.8f + 32.0f, tm.tm_hour, tm.tm_min);
    sprintf(lcdLine1, "%.0fC %.1fF %02d:%02d\n", temperature, temperature * 1.8f + 32.0f, (int) runTimeHour, (int) runTimeMin);
    printf("Input Supply Voltage = %.2fV, Current = %dmA\n", voltage_sint, current_sint);
    printf("Supply1 Voltage = %.2fV, Current = %dmA\n", voltage_s1, current_s1);
    printf("Supply2 Voltage = %.2fV, Current = %dmA\n", voltage_s2, current_s2);
    printf("Supply3 Voltage = %.2fV, Current = %dmA\n", voltage_s3, current_s3);
    //sprintf(lcdLine2, "V%.2f %.2f %.2f", voltage_sint, voltage_s2, voltage_s3);

    //sprintf(lcdLine2, "Pass 0x%0x", passNum++);
    if (passNum%5 == 0) {
      printf("Updating LCD\n");
      lcdUpdate(lcdLine1, lcdLine2);
    }
    //printf("Got button \"%c\"\n", getButton() );

    usleep(100000);
  }
}


