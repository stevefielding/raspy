// ------------------ softKey.h --------------------------

// "a" text position state
#define A_TEMP_INC_STATE 0
#define A_TEMP_DEC_STATE 1
#define A_TEMP_CTRL_MODE_STATE 2
#define A_TEMP_FORMAT_STATE 3
#define A_MESSAGE_STATE 4
#define A_MUSIC_STATE 5
#define A_TEETH_STATE 6
#define A_TABLET_STATE 7
#define A_BANDS_STATE 8
#define A_STRETCH_STATE 9
#define FIRST_A_STATE A_TEMP_INC_STATE
#define LAST_A_STATE A_STRETCH_STATE

// "a" text. 8 char long
#define A_TEMP_INC_TEXT "Temp+"
#define A_TEMP_DEC_TEXT "Temp-"
#define A_TEMP_CTRL_MODE_STATE_TEXT "Mode"
#define A_TEMP_FORMAT_STATE_TEXT "TempFmt"
#define A_MESSAGE_STATE_TEXT "Message"
#define A_MUSIC_TEXT "Music"
#define A_TEETH_TEXT "Teeth"
#define A_TABLET_TEXT "Tablet"
#define A_BANDS_TEXT "Bands"
#define A_STRETCH_TEXT "Stretch"

// temp control modes
#define OFF_CTRL_MODE 0
#define HEAT_CTRL_MODE 1
#define COOL_CTRL_MODE 2
#define FAN_CTRL_MODE 3

// temp control mode text
#define OFF_CTRL_MODE_TEXT "Off"
#define HEAT_CTRL_MODE_TEXT "Heat"
#define COOL_CTRL_MODE_TEXT "AC"
#define FAN_CTRL_MODE_TEXT "Fan"

// temp format
#define FAHRENHEIT_DISP_MODE 0
#define CELSIUS_DISP_MODE 1

// temp format text
#define A_TEMP_FORMAT_STATE_TEXT_F "Fahr"
#define A_TEMP_FORMAT_STATE_TEXT_C "Cels"

// message states
#define OFF_MESSAGE 0
#define READ_MESSAGE 1
#define BIRTHDAY_MESSAGE 2

// message states text
#define OFF_MESSAGE_TEXT "Off"
#define READ_MESSAGE_TEXT "Read"
#define BIRTHDAY_MESSAGE_TEXT "Bthday"

// music states
#define NO_MUSIC 0
#define MUSIC_1 1
#define MUSIC_2 2
#define MUSIC_3 3
#define MUSIC_4 4

// message states text
#define NO_MUSIC_TEXT "Off"
#define MUSIC_1_TEXT "Music1"
#define MUSIC_2_TEXT "Music2"
#define MUSIC_3_TEXT "Music3"
#define MUSIC_4_TEXT "Music4"

//alert states
#define ALERT_OFF 0
#define ALERT_ON 1

//alert states text
#define ALERT_OFF_TEXT "Off"
#define ALERT_ON_TEXT "On"

struct sysState {
  float tempSetPoint;
  int tempCtrlMode;
  int tempFormat;
  int message;
  int music;
  int teeth;
  int tablet;
  int bands;
  int stretch;
};

int softKeyUpdate(char *lcdLine, unsigned char keyp, struct sysState *mySysState);

#define SOFT_KEY_DISP_TOUT 5 // 5 second
