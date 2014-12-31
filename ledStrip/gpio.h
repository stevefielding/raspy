#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define RASPI_PCB_REV2



// GPIO numbers
#define SUPPLY_AC_EN 18
#ifdef RASPI_PCB_REV2
  #define SUPPLY_1_EN 27
#else
  #define SUPPLY_1_EN 17
#endif
#define SUPPLY_2_EN 23
#define SUPPLY_3_EN 17
#define PIR_DET 25
#define SPI_DOUT 10
#define SPI_CLK 11
#define HEAT_EN SUPPLY_1_EN
#define COOL_EN SUPPLY_2_EN
#define FAN_EN SUPPLY_3_EN

// permissible temp control modes
#define DISABLE_HEAT_COOL_FAN 0
#define HEAT_ACTIVE 1
#define COOL_ACTIVE 2
#define FAN_ONLY_ACTIVE 3

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

//#define DEBUG_DELAY_MODE
#ifdef DEBUG_DELAY_MODE
  #define FAN_LAG_TIME 3 
  #define FAN_LEAD_TIME 2 
  #define OFF_DELAY_TIME 5 
  #define ON_MIN_TIME 6 
#else
  #define FAN_LAG_TIME 300 //fan off 300 seconds after heat cool completed
  #define FAN_LEAD_TIME 20 //fan on 20 seconds before heat cool started
  #define OFF_DELAY_TIME 5 //everything off for 5 seconds before restarting
  #define ON_MIN_TIME 60 // miniumum on time for heat and cool
#endif

#define TEMPERATURE_HYST 0.5f   //0.5 Fahrenheit of hysteresis

void setup_io();
void gpioInit();
extern void putsSPI2(unsigned int numBytes, unsigned int txData[]);


