// ------------------------------------------- i2cUtil.c -------------------------------------- //
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <linux/fs.h>
#include "i2cUtil.h"
#include <sys/ioctl.h>

void setI2cAddress (int fd, int address) {
  if (ioctl(fd, I2C_SLAVE, address) < 0) {
    printf("ERROR: setI2cAddress\n");
    exit(1);
  }
}

unsigned int I2CWriteReg16 (int fd, unsigned char i2cAddress, unsigned char i2cReg, unsigned int data) {
unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE];
  setI2cAddress(fd, i2cAddress);
  I2C_WR_Buf[0] = i2cReg;
  I2C_WR_Buf[1] = (data >> 8) & 0xff;
  I2C_WR_Buf[2] = data & 0xff;
  if(write(fd, I2C_WR_Buf,3) != 3) {
    perror("Write Error");
  }
  return 0;
}

unsigned int I2CWriteReg8 (int fd, unsigned char i2cAddress, unsigned char i2cReg, unsigned char data) {
unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE];
  setI2cAddress(fd, i2cAddress);
  I2C_WR_Buf[0] = i2cReg;
  I2C_WR_Buf[1] = data;
  if(write(fd, I2C_WR_Buf,2) != 2) {
    perror("Write Error");
  }
  return 0;
}

unsigned int I2CReadReg16 (int fd, unsigned char i2cAddress, unsigned char i2cReg) {

unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE];
unsigned char I2C_RD_Buf[MAX_BUFFER_SIZE];

  I2C_WR_Buf[0] = i2cReg;
  setI2cAddress(fd, i2cAddress);
  if(write(fd, I2C_WR_Buf, 1) != 1) {
    perror("Write Error");
  }
  setI2cAddress(fd, i2cAddress);                              // This address might not need to be set again but this works for me
  if(read(fd, I2C_RD_Buf, 2) != 2) {
    perror("Read Error");
  }
  return ((I2C_RD_Buf[0] << 8) + I2C_RD_Buf[1]);
}

unsigned char I2CReadReg8 (int fd, unsigned char i2cAddress, unsigned char i2cReg) {

unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE];
unsigned char I2C_RD_Buf[MAX_BUFFER_SIZE];

  I2C_WR_Buf[0] = i2cReg;
  setI2cAddress(fd, i2cAddress);
  if(write(fd, I2C_WR_Buf, 1) != 1) {
    perror("Write Error");
  }
  if(read(fd, I2C_RD_Buf, 8) != 8) {
    perror("Read Error");
  }
  return (I2C_RD_Buf[0]);
}



int twosComplement (int val) {
  //printf("val = 0x%0x\n", val);
  if ((val & 0x8000) == 0) return val;
  else return (((val & 0x7fff) - 1 ) ^ 0x7fff);
}

unsigned int ina226Init(int fd, unsigned char i2cAddress) {
    unsigned int readData = 0;
    unsigned int writeData;
    unsigned int testFail = 0;

    printf("Configuring INA226\n");
    writeData = 0x8000;    
    I2CWriteReg16(fd, i2cAddress, INA226_CONF_REG, writeData); //reset
    writeData = 0x4f27;
    I2CWriteReg16(fd, i2cAddress, INA226_CONF_REG, writeData); //average 1024 samples, bus and shunt 1.1mS conversion time, continuous conversions
    writeData = 0x1400;
    I2CWriteReg16(fd, i2cAddress, INA226_CAL_REG, writeData);   // configure for 100uV/bit, 0.01R sense resistor. CAL = 0.00512 / (100uV/bit * 0.01R) = 0x1400
    readData = I2CReadReg16(fd, i2cAddress, INA226_CONF_REG);
    printf("Config reg = 0x%0x\n", readData);
    if (readData != 0x4f27) testFail = 1;
    readData = I2CReadReg16(fd, i2cAddress, INA226_CAL_REG);
    printf("Cal reg = 0x%0x\n", readData);
    if (readData != 0x1400) testFail = 1;
    readData = I2CReadReg16(fd, i2cAddress, INA226_DIE_ID_REG);
    printf("Die ID reg = 0x%0x\n", readData);

    if (testFail == 0)
      printf("--- INA226 Init Test passed\n");
    else 
      printf("--- INA226 Init Test failed\n");
    printf("\n");
    return testFail;
}

unsigned int ad7410Init(int fd, unsigned char i2cAddress) {
    unsigned int readData = 0;
    unsigned char writeData;
    unsigned int testFail = 0;

    printf("Configuring ADT7410\n");
    writeData = 0x00;    
    I2CWriteReg8(fd, i2cAddress, ADT7410_RESET, writeData); //reset
    sleep (1);
    writeData = 0x80;    
    I2CWriteReg8(fd, i2cAddress, ADT7410_CONF, writeData); //configure 16-bit temperature reading
    readData = I2CReadReg16(fd, i2cAddress, ADT7410_TEMP_MSB);
    printf("ADT7410_TEMP = 0x%0x\n", readData);
    printf("Temp = %.2f C\n", readData * 0.0078f);
    //readData = I2CReadReg8(fd, i2cAddress, ADT7410_TEMP_LSB);
    //printf("ADT7410_TEMP_LSB = 0x%0x\n", readData);
    //readData = I2CReadReg8(fd, i2cAddress, ADT7410_STATUS);
    //printf("ADT7410_STATUS = 0x%0x\n", readData);
    //readData = I2CReadReg8(fd, i2cAddress, ADT7410_CONF);
    //printf("ADT7410_CONF = 0x%0x\n", readData);
    //readData = I2CReadReg8(fd, i2cAddress, ADT7410_ID);
    //if (readData != 0xcc) testFail = 1;
    //printf("ID reg = 0x%0x\n", readData);


    if (testFail == 0)
      printf("--- ADT7410 Init Test passed\n");
    else 
      printf("--- ADT7410 Init Test failed\n");
    printf("\n");
    return testFail;
}

void ina226GetMeas (int fd, unsigned char i2cAddress, char *desc, float *busVolt, unsigned int *current) {
    unsigned int readData = 0;

    //printf("INA226 measurements: %s\n", desc);
    readData = I2CReadReg16(fd, i2cAddress, INA226_SHUNT_VOLT_REG);
    readData = twosComplement(readData);
    //printf("  Shunt Voltage = 0x%0x = %d uVolts\n", readData, (unsigned int) ((float) readData * 2.5f) );
    readData = I2CReadReg16(fd, i2cAddress, INA226_BUS_VOLT_REG);
    readData = twosComplement(readData);
    //printf("  Bus Voltage = 0x%0x = %.2f Volts\n", readData,  ((float) readData * 0.00125f) );
    *busVolt =  ((float) readData * 0.00125f);
    readData = I2CReadReg16(fd, i2cAddress, INA226_POWER_REG);
    //printf("  Power = 0x%0x = %d mW\n", readData, (unsigned int) ((float) readData * 2.5f) );
    readData = I2CReadReg16(fd, i2cAddress, INA226_CURRENT_REG);
    readData = twosComplement(readData);
    //printf("  Current = 0x%0x = %d mA\n", readData, (unsigned int) ((float) readData * 0.1f) );
    *current = (unsigned int) ((float) readData * 0.1f);
}

float ad7410GetTemp(int fd, unsigned char i2cAddress) {
    unsigned int readData = 0;
    float temperature;

    readData = I2CReadReg16(fd, i2cAddress, ADT7410_TEMP_MSB);
    //printf("ADT7410_TEMP = 0x%0x\n", readData);
    temperature = readData * 0.0078f;
    return temperature;
}



