#define DEBUG_PRINT

#define LCD_LINE_LEN 16
#define LCD_CLEAR_DISPLAY   1
#define LCD20X4_LINE1_START 128
#define LCD20X4_LINE2_START 192
#define LCD20X4_LINE3_START 148
#define LCD20X4_LINE4_START 212
#define LCD16X2_LINE1_START 128
#define LCD16X2_LINE2_START 192

#define USART_LCD_SPEED			B9600
//#define USART_LCD_SPEED			B19200
//#define USART_LCD_SPEED			B57600

// uart command bytes
#define CMD_0X7C 0x7c
#define CMD_LCD 0xfe
#define CMD_RED_BL 0xfd
#define CMD_GREEN_BL 0xfc
#define CMD_BLUE_BL 0xfb
#define CMD_BI_COL_LED 0xfa
#define CMD_LCD_INIT 0xf9 //need to wait for 9mS for completion

// LED blink states
#define RED_LED_DARK 0x00
#define RED_LED_BLINK 0x01
#define RED_LED_STEADY 0x02
#define GREEN_LED_DARK 0x00
#define GREEN_LED_BLINK 0x10
#define GREEN_LED_STEADY 0x20

// function prototypes
void sendCmd(unsigned char cmd, unsigned char dataVal);
void lcdInit(void);
void lcdUpdate(char *lcdLine1, char *lcdLine2);
void setbackLight(unsigned char red, unsigned char green, unsigned char blue);
unsigned char getButton();
void backLightRamp(unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char redStep, unsigned char greenStep, unsigned char blueStep);
void backLightTemp(float temperature, unsigned char *red, unsigned char *green, unsigned char *blue);
void backLightFlash(unsigned char *red, unsigned char *green, unsigned char *blue);
void backLightFlash2(unsigned char *red, unsigned char *green, unsigned char *blue);
void ledTempState (int tempCtrlMode, int heatActive, int coolActive);

// temperature limits used to control backlight color
#define MIN_TEMP_BL 67.0f
#define MAX_TEMP_BL 83.0f
#define SCALE_TEMP_BL 255.0f/(MAX_TEMP_BL-MIN_TEMP_BL)

