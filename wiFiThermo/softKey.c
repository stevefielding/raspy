// ------------------ softKey.c --------------------------
#include "softKey.h"
#include "lcdUtil.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

int softKeysActive=0;
int aPosState = A_TEMP_INC_STATE;
long int start_time;
long int time_difference;

int softKeyUpdate(char *lcdLine, unsigned char keyp, struct sysState *mySysState)
{
struct timespec gettime_now;
int aTextStart;
int bTextStart;
int aIndex;
int bIndex;
int lineIndex;
char aText[LCD_LINE_LEN+1];
char bText[LCD_LINE_LEN+1];

  if (keyp == 'a' || keyp == 'b') {
    if (softKeysActive == 0) {
      //mySysState->message = OFF_MESSAGE; //turn off message as soon as key is pressed
    }
    else {
      // update "a" position state
      if (keyp == 'a') {
        if (aPosState == LAST_A_STATE)
          aPosState = FIRST_A_STATE;
        else
          aPosState++;
      }

      // update "b" position state
      if (keyp == 'b') {
        switch (aPosState) {
          case A_TEMP_INC_STATE: 
            mySysState->tempSetPoint += 1.0f;
            break;
          case A_TEMP_DEC_STATE: 
            mySysState->tempSetPoint -= 1.0f;
            break;
          case A_TEMP_CTRL_MODE_STATE:
            if (mySysState->tempCtrlMode == FAN_CTRL_MODE)
              mySysState->tempCtrlMode = OFF_CTRL_MODE;
            else
              mySysState->tempCtrlMode += 1;
            break;
          case A_TEMP_FORMAT_STATE:
            if (mySysState->tempFormat == FAHRENHEIT_DISP_MODE)
              mySysState->tempFormat = CELSIUS_DISP_MODE;
            else
              mySysState->tempFormat = FAHRENHEIT_DISP_MODE;
            break;
          case A_MESSAGE_STATE:
            if (mySysState->message == BIRTHDAY_MESSAGE)
              mySysState->message = OFF_MESSAGE;
            else
              mySysState->message += 1;
            break;
          case A_MUSIC_STATE:
            if (mySysState->music == MUSIC_4)
              mySysState->music = NO_MUSIC;
            else
              mySysState->music += 1;
            break;
          case A_TEETH_STATE:
            if (mySysState->teeth == ALERT_OFF)
              mySysState->teeth = ALERT_ON;
            else
              mySysState->teeth = ALERT_OFF;
            break;
          case A_TABLET_STATE:
            if (mySysState->tablet == ALERT_OFF)
              mySysState->tablet = ALERT_ON;
            else
              mySysState->tablet = ALERT_OFF;
            break;
          case A_BANDS_STATE:
            if (mySysState->bands == ALERT_OFF)
              mySysState->bands = ALERT_ON;
            else
              mySysState->bands = ALERT_OFF;
            break;
          case A_STRETCH_STATE:
            if (mySysState->stretch == ALERT_OFF)
              mySysState->stretch = ALERT_ON;
            else
              mySysState->stretch = ALERT_OFF;
            break;
        }
      }
    }

    // update "a" and "b" soft key text
    switch (aPosState) {
      case A_TEMP_INC_STATE:
        strcpy(aText, A_TEMP_INC_TEXT);
        sprintf(bText, "%.1f", mySysState->tempSetPoint);
        break;
      case A_TEMP_DEC_STATE: 
        strcpy(aText, A_TEMP_DEC_TEXT);
        sprintf(bText, "%.1f", mySysState->tempSetPoint);
        break;
      case A_TEMP_CTRL_MODE_STATE:
        strcpy(aText, A_TEMP_CTRL_MODE_STATE_TEXT);
        switch(mySysState->tempCtrlMode) {
          case (OFF_CTRL_MODE): strcpy(bText, OFF_CTRL_MODE_TEXT); break;
          case (HEAT_CTRL_MODE): strcpy(bText, HEAT_CTRL_MODE_TEXT); break;
          case (COOL_CTRL_MODE): strcpy(bText, COOL_CTRL_MODE_TEXT); break;
          case (FAN_CTRL_MODE): strcpy(bText, FAN_CTRL_MODE_TEXT); break;
          default: strcpy(bText, "Error"); break;
        }
        break;
      case A_TEMP_FORMAT_STATE:
        strcpy(aText, A_TEMP_FORMAT_STATE_TEXT);
        if (mySysState->tempFormat == FAHRENHEIT_DISP_MODE)
          strcpy(bText, A_TEMP_FORMAT_STATE_TEXT_F);
        else
          strcpy(bText, A_TEMP_FORMAT_STATE_TEXT_C);
        break;
      case A_MESSAGE_STATE:
        strcpy(aText, A_MESSAGE_STATE_TEXT);
        switch(mySysState->message) {
          case (OFF_MESSAGE): strcpy(bText, OFF_MESSAGE_TEXT); break;
          case (READ_MESSAGE): strcpy(bText, READ_MESSAGE_TEXT); break;
          case (BIRTHDAY_MESSAGE): strcpy(bText, BIRTHDAY_MESSAGE_TEXT); break;
          default: strcpy(bText, "Error"); break;
        }
        break;
      case A_MUSIC_STATE:
        strcpy(aText, A_MUSIC_TEXT);
        switch(mySysState->music) {
          case (NO_MUSIC): strcpy(bText, NO_MUSIC_TEXT); break;
          case (MUSIC_1): strcpy(bText, MUSIC_1_TEXT); break;
          case (MUSIC_2): strcpy(bText, MUSIC_2_TEXT); break;
          case (MUSIC_3): strcpy(bText, MUSIC_3_TEXT); break;
          case (MUSIC_4): strcpy(bText, MUSIC_4_TEXT); break;
          default: strcpy(bText, "Error"); break;
        }
        break;
      case A_TEETH_STATE:
        strcpy(aText, A_TEETH_TEXT);
        if (mySysState->teeth == ALERT_OFF)
          strcpy(bText, ALERT_OFF_TEXT);
        else
          strcpy(bText, ALERT_ON_TEXT);
        break;
      case A_TABLET_STATE:
        strcpy(aText, A_TABLET_TEXT);
        if (mySysState->tablet == ALERT_OFF)
          strcpy(bText, ALERT_OFF_TEXT);
        else
          strcpy(bText, ALERT_ON_TEXT);
        break;
      case A_BANDS_STATE:
        strcpy(aText, A_BANDS_TEXT);
        if (mySysState->bands == ALERT_OFF)
          strcpy(bText, ALERT_OFF_TEXT);
        else
          strcpy(bText, ALERT_ON_TEXT);
        break;
      case A_STRETCH_STATE:
        strcpy(aText, A_STRETCH_TEXT);
        if (mySysState->stretch == ALERT_OFF)
          strcpy(bText, ALERT_OFF_TEXT);
        else
          strcpy(bText, ALERT_ON_TEXT);
        break;
      default:
        strcpy(aText, "Unused");
        strcpy(bText, "state");
        break;
    }

    // Combine "a" text and "b" text and align text on LCD bottom line
    aTextStart = ((LCD_LINE_LEN/2) - strlen(aText)) / 2;
    bTextStart = (((LCD_LINE_LEN/2) - strlen(bText)) / 2) + (LCD_LINE_LEN/2);
    aIndex = 0;
    bIndex = 0;
    for (lineIndex = 0; lineIndex < LCD_LINE_LEN; lineIndex++) {
      if (lineIndex < aTextStart) {lcdLine[lineIndex] = ' ';}
      else if (lineIndex < aTextStart + strlen(aText) ) {lcdLine[lineIndex] = aText[aIndex++];}
      else if (lineIndex < bTextStart) {lcdLine[lineIndex] = ' ';}
      else if (lineIndex < bTextStart + strlen(bText) ) {lcdLine[lineIndex] = bText[bIndex++];}
      else {lcdLine[lineIndex] = ' ';}
    }

    // make soft keys active
    softKeysActive = 1;
    clock_gettime(CLOCK_REALTIME, &gettime_now);
    start_time = gettime_now.tv_sec;		//Get time
    time_difference = 0;
  }
  else {
    if (time_difference >= SOFT_KEY_DISP_TOUT)
      softKeysActive = 0;
    else {
      //time out to inactive
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      time_difference = gettime_now.tv_sec - start_time;
    }
  }
  return softKeysActive;
}


