#include "i2c.h"

void InitI2C(void)
{
    TRISC = 0x18; //RC3, RC4 input
    SSPADD =0x27; 
    SSPSTAT = 0x80;
    SSPCON1 = 0x38;
}

void I2C_Idle(void)
{
  while(READ_WRITE);
  ZERO=0;
  while(ZERO)
  {
    SSPCON2&0x1f;
  }
}

void I2C_IntWait(void)
{
  while(!SSPIF);
  SSPIF=0;
}

void I2C_StartCondition(void)
{
  I2C_Idle();
  SEN=1;
  I2C_IntWait();
}

void I2C_StopCondition(void)
{
  PEN = 1;
  I2C_IntWait();
}

bit I2C_Write_Byte(unsigned char Byte)
{
  SSPBUF = Byte;
  I2C_IntWait();
  return ACKSTAT;
}

void I2C_SendByteByADDR(unsigned char c,unsigned char addr)
{
  I2C_StartCondition();
  I2C_Write_Byte(addr);
  I2C_Write_Byte(c);
  I2C_StopCondition();
}
unsigned char I2C_ReceiveByte_Ack(void)
{
  unsigned char data = 0;
  RCEN=1;
  while(!SSPIF);
  data=SSPBUF;
  I2C_IntWait();
  ACKDT=0;
  ACKEN=1;
  I2C_IntWait();
  return data;
}

unsigned char I2C_ReceiveByte_Nack(void)
{
  unsigned char data = 0;
  RCEN=1;
  while(!SSPIF);
  data=SSPBUF;
  while(!SSPIF);
  SSPIF=0;
  ACKDT=1;
  ACKEN=1;
  while(!SSPIF);
  SSPIF=0;
  return data;
}