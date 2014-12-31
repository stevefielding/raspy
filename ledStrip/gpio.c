// ----------------------------------------------------------------
// -------------------- gpio.c --------------------------------
// ----------------------------------------------------------------

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "gpio.h"
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
  INP_GPIO(SPI_DOUT);
  OUT_GPIO(SPI_DOUT);
  INP_GPIO(SPI_CLK);
  OUT_GPIO(SPI_CLK);
  GPIO_CLR = 1<<SPI_CLK;
  GPIO_CLR = 1<<SPI_DOUT;

}

// ------------------------------ putSPI2 ----------------------------------
void putsSPI2(unsigned int numBytes, unsigned int txData[]) {
  unsigned int i, j;
  unsigned int txByte;
  unsigned int mask;

  for (i=0; i< numBytes; i++) {
    mask = 0x80;
    txByte = txData[i];
    for (j=0;j<8;j++) {
      if ((txByte & mask) == 0x0)
        GPIO_CLR = 1<<SPI_DOUT;
      else
        GPIO_SET = 1<<SPI_DOUT;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_CLR = 1<<SPI_CLK;
      GPIO_SET = 1<<SPI_CLK;
      GPIO_SET = 1<<SPI_CLK;
      GPIO_SET = 1<<SPI_CLK;
      GPIO_SET = 1<<SPI_CLK;
      GPIO_SET = 1<<SPI_CLK;
      mask = mask >> 1;
    }
  }
}

