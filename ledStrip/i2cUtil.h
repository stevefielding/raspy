// ------------------------------------------- i2cUtil.h -------------------------------------- //

/* INA226 Registers */
#define INA226_CONF_REG       0x00
#define INA226_SHUNT_VOLT_REG 0x01
#define INA226_BUS_VOLT_REG   0x02
#define INA226_POWER_REG      0x03
#define INA226_CURRENT_REG    0x04
#define INA226_CAL_REG        0x05
#define INA226_MASK_EN_REG    0x06
#define INA226_ALERT_LIM_REG  0x07
#define INA226_DIE_ID_REG     0xff

#define MAX_BUFFER_SIZE 8

/* ADT7410 Registers */
#define ADT7410_TEMP_MSB      0x00
#define ADT7410_TEMP_LSB      0x01
#define ADT7410_STATUS        0x02
#define ADT7410_CONF          0x03
#define ADT7410_ID            0x0b
#define ADT7410_RESET         0x2f

void setI2cAddress (int fd, int address);
unsigned int I2CWriteReg16 (int fd, unsigned char i2cAddress, unsigned char i2cReg, unsigned int data);
unsigned int I2CWriteReg8 (int fd, unsigned char i2cAddress, unsigned char i2cReg, unsigned char data);
unsigned int I2CReadReg16 (int fd, unsigned char i2cAddress, unsigned char i2cReg);
unsigned char I2CReadReg8 (int fd, unsigned char i2cAddress, unsigned char i2cReg);
int twosComplement (int val);
unsigned int ina226Init(int fd, unsigned char i2cAddress);
unsigned int ad7410Init(int fd, unsigned char i2cAddress);
void ina226GetMeas (int fd, unsigned char i2cAddress, char *desc, float *voltage, unsigned int *current);
float ad7410GetTemp(int fd, unsigned char i2cAddress);



