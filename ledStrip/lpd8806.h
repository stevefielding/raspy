
#define LED_STRIP_LEN 32 // number of LEDs on the strip
//#define RAND_MAX 255
#define LED_STRIP_OFF 0
#define LED_STRIP_RAND 1
#define LED_STRIP_RED_WHITE 2
#define LED_STRIP_GREEN_WHITE 3
#define LED_STRIP_RED 4
#define LED_STRIP_WHITE 5

extern void write_latch(unsigned int len);
extern void send_frame(unsigned long int led_strip_colors[], unsigned int ledStripLen, unsigned int litLen);
extern void send_frame_oneColor(unsigned int red, unsigned int green, unsigned int blue, unsigned int ledStripLen, unsigned int litLen);
void blendPixels(unsigned long int led_strip_colors1[], unsigned long int led_strip_colors2[], unsigned int stepNum, unsigned int delayVal);


