#ifndef I2C_H
#define I2C_H
//--------------------------------------------------------------
#include <xc.h>
//--------------------------------------------------------------
#define _XTAL_FREQ 4000000
void InitI2C(void);
void I2C_Idle(void);
void I2C_IntWait(void);
bit I2C_Write_Byte(unsigned char Byte);
void I2C_SendByteByADDR(unsigned char c,unsigned char addr);
unsigned char I2C_ReceiveByte_Nack(void);
unsigned char I2C_ReceiveByte_Ack(void);
void I2C_StartCondition(void);
void I2C_StopCondition(void);
//--------------------------------------------------------------
#endif /* I2C_H */