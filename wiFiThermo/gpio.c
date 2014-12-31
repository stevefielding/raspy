// ----------------------------------------------------------------
// -------------------- gpio.c --------------------------------
// ----------------------------------------------------------------

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "gpio.h"
#include "softKey.h"
#include "lcdUtil.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0


// --------------------------------- setup_io -----------------------------------//
//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


} // setup_io

// --------------------------------- gpioInit -----------------------------------//
void gpioInit()
{
  printf("gpioInit: Started\n");

  setup_io();

  // Set GPIO pins 
  INP_GPIO(SUPPLY_AC_EN); // must use INP_GPIO before we can use OUT_GPIO
  OUT_GPIO(SUPPLY_AC_EN);
  INP_GPIO(SUPPLY_1_EN);
  OUT_GPIO(SUPPLY_1_EN);
  INP_GPIO(SUPPLY_2_EN);
  OUT_GPIO(SUPPLY_2_EN);
  INP_GPIO(SUPPLY_3_EN);
  OUT_GPIO(SUPPLY_3_EN);
  INP_GPIO(PIR_DET);
}

// --------------------------------- setTempCtrlSource -----------------------------------//
void setTempCtrlSource(int ctrlSource)
{
  switch (ctrlSource) {
    case DISABLE_HEAT_COOL_FAN:
      GPIO_CLR = 1<<HEAT_EN;
      GPIO_CLR = 1<<COOL_EN;
      GPIO_CLR = 1<<FAN_EN;
      GPIO_CLR = 1<<SUPPLY_AC_EN;  //debug only
      break;
    case FAN_ONLY_ACTIVE:
      GPIO_CLR = 1<<HEAT_EN;
      GPIO_CLR = 1<<COOL_EN;
      GPIO_SET = 1<<FAN_EN;
      break;
    case HEAT_ACTIVE:
      GPIO_SET = 1<<HEAT_EN;
      GPIO_CLR = 1<<COOL_EN;
      GPIO_SET = 1<<FAN_EN;
      GPIO_SET = 1<<SUPPLY_AC_EN;  //debug only
      break;
    case COOL_ACTIVE:
      GPIO_CLR = 1<<HEAT_EN;
      GPIO_SET = 1<<COOL_EN;
      GPIO_SET = 1<<FAN_EN;
      break;
    default:
      GPIO_CLR = 1<<HEAT_EN;
      GPIO_CLR = 1<<COOL_EN;
      GPIO_CLR = 1<<FAN_EN;
      break;
  }
}

#define TEMP_OFF_STATE 0
#define TEMP_COOL_START_STATE 1
#define TEMP_HEAT_START_STATE 2
#define TEMP_HEAT_STATE 3
#define TEMP_COOL_STATE 4
#define TEMP_FAN_START_STATE 5
#define TEMP_FAN_STOP_STATE 6
#define TEMP_OFF_DELAY_STATE 7
// --------------------------------- tempCtrl -----------------------------------//
void tempCtrl(int tempCtrlMode, float tempSetPoint, float temperature, int *heatActive, int *coolActive)
{
static int tempCtrlCurrState = TEMP_OFF_STATE;
static long int startTime;
static long int timeDelay;
static long int timeDifference=0;
struct timespec gettime_now;

  switch (tempCtrlCurrState) {

    // --------------- TEMP_OFF_STATE
    case TEMP_OFF_STATE:
      printf("TEMP_OFF_STATE\n");
      setTempCtrlSource(DISABLE_HEAT_COOL_FAN);
      *heatActive = 0;
      *coolActive = 0;
      switch (tempCtrlMode) {
        case HEAT_CTRL_MODE:
          if (temperature < tempSetPoint - TEMPERATURE_HYST) {
            tempCtrlCurrState = TEMP_FAN_START_STATE;
            timeDelay = FAN_LEAD_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          break;
        case COOL_CTRL_MODE:
          if (temperature > tempSetPoint + TEMPERATURE_HYST) {
            tempCtrlCurrState = TEMP_FAN_START_STATE;
            timeDelay = FAN_LEAD_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          break;
        case FAN_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          break;
        default:
          tempCtrlCurrState = TEMP_OFF_STATE;
          break;
      }
      break;

    // --------------- TEMP_HEAT_STATE
    case TEMP_HEAT_STATE:
      printf("TEMP_HEAT_STATE\n");
      setTempCtrlSource(HEAT_ACTIVE);
      *heatActive = 1;
      timeDifference = 0;
      switch (tempCtrlMode) {
        case HEAT_CTRL_MODE:
          if (temperature > tempSetPoint) {
            tempCtrlCurrState = TEMP_FAN_STOP_STATE;
            timeDelay = FAN_LAG_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          break;
        case OFF_CTRL_MODE:
        case COOL_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          timeDelay = FAN_LAG_TIME;
          clock_gettime(CLOCK_REALTIME, &gettime_now);
          startTime = gettime_now.tv_sec;
          break;
        case FAN_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          break;
        default:
          tempCtrlCurrState = TEMP_OFF_STATE;
          break;
      }
      break;

    // --------------- TEMP_COOL_STATE
    case TEMP_COOL_STATE:
      printf("TEMP_COOL_STATE\n");
      setTempCtrlSource(COOL_ACTIVE);
      *coolActive = 1;
      timeDifference = 0;
      switch (tempCtrlMode) {
        case COOL_CTRL_MODE:
          if (temperature < tempSetPoint) {
            tempCtrlCurrState = TEMP_FAN_STOP_STATE;
            timeDelay = FAN_LAG_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          break;
        case OFF_CTRL_MODE:
        case HEAT_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          clock_gettime(CLOCK_REALTIME, &gettime_now);
          startTime = gettime_now.tv_sec;
          timeDelay = FAN_LAG_TIME;
          break;
        case FAN_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          break;
        default:
          tempCtrlCurrState = TEMP_OFF_STATE;
          break;
      }
      break;


    // --------------- TEMP_FAN_START_STATE
    case TEMP_FAN_START_STATE:
      printf("TEMP_FAN_START_STATE\n");
      setTempCtrlSource(FAN_ONLY_ACTIVE);
      *heatActive = 0;
      *coolActive = 0;
      switch (tempCtrlMode) {
        case FAN_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          break;
        case COOL_CTRL_MODE:
        case OFF_CTRL_MODE:
        case HEAT_CTRL_MODE:
          if (timeDifference >= timeDelay) {
            switch (tempCtrlMode) {
              case COOL_CTRL_MODE: tempCtrlCurrState = TEMP_COOL_START_STATE; break;
              case HEAT_CTRL_MODE: tempCtrlCurrState = TEMP_HEAT_START_STATE; break;
              default: tempCtrlCurrState = TEMP_OFF_DELAY_STATE; break;
            }
            timeDifference = 0;
            timeDelay = ON_MIN_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          else {
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            timeDifference = gettime_now.tv_sec - startTime;
          }
          break;
        default:
          tempCtrlCurrState = TEMP_OFF_STATE;
          break;
      }
      break;



    // --------------- TEMP_FAN_STOP_STATE
    case TEMP_FAN_STOP_STATE:
      printf("TEMP_FAN_STOP_STATE\n");
      setTempCtrlSource(FAN_ONLY_ACTIVE);
      *heatActive = 0;
      *coolActive = 0;
      switch (tempCtrlMode) {
        case FAN_CTRL_MODE:
          tempCtrlCurrState = TEMP_FAN_STOP_STATE;
          break;
        case COOL_CTRL_MODE:
        case OFF_CTRL_MODE:
        case HEAT_CTRL_MODE:
          if (timeDifference >= timeDelay) {
            tempCtrlCurrState = TEMP_OFF_DELAY_STATE;
            timeDifference = 0;
            timeDelay = OFF_DELAY_TIME;
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            startTime = gettime_now.tv_sec;
          }
          else {
            clock_gettime(CLOCK_REALTIME, &gettime_now);
            timeDifference = gettime_now.tv_sec - startTime;
          }
          break;
        default:
          tempCtrlCurrState = TEMP_OFF_STATE;
          break;
      }
      break;

    // --------------- TEMP_OFF_DELAY_STATE
    case TEMP_OFF_DELAY_STATE:
      printf("TEMP_OFF_DELAY_STATE\n");
      setTempCtrlSource(DISABLE_HEAT_COOL_FAN);
      *heatActive = 0;
      *coolActive = 0;
      if (timeDifference >= timeDelay)
        tempCtrlCurrState = TEMP_OFF_STATE;
      else {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        timeDifference = gettime_now.tv_sec - startTime;
      }
      break;

    // --------------- TEMP_COOL_START_STATE
    case TEMP_COOL_START_STATE:
      printf("TEMP_COOL_START_STATE\n");
      setTempCtrlSource(COOL_ACTIVE);
      *coolActive = 1;
      if (timeDifference >= timeDelay)
        tempCtrlCurrState = TEMP_COOL_STATE;
      else {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        timeDifference = gettime_now.tv_sec - startTime;
      }
      break;

    // --------------- TEMP_HEAT_START_STATE
    case TEMP_HEAT_START_STATE:
      printf("TEMP_HEAT_START_STATE\n");
      setTempCtrlSource(HEAT_ACTIVE);
      *heatActive = 1;
      if (timeDifference >= timeDelay)
        tempCtrlCurrState = TEMP_HEAT_STATE;
      else {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        timeDifference = gettime_now.tv_sec - startTime;
      }
      break;

    // -------------- default
    default:
      tempCtrlCurrState = TEMP_OFF_STATE;
      *heatActive = 0;
      *coolActive = 0;
      break;
  }
}

