#ifndef MT107T_I2C_H
#define	MT107T_I2C_H
#include <xc.h>
#include "i2c.h"

#define _XTAL_FREQ 4000000
#define A0_ON      I2C_SendByteByADDR(buffer |= 0b00000001, 0x70)
#define A0_OFF     I2C_SendByteByADDR(buffer &= 0b11111100, 0x70) 
#define WR1_ON     I2C_SendByteByADDR(buffer |= 0b00000100, 0x70)
#define WR1_OFF    I2C_SendByteByADDR(buffer &= 0b11111001, 0x70)
#define CLR_buffer I2C_SendByteByADDR(buffer &= 0b00000111, 0x70)
void I2C_Send_Data(char adr, char L_Data, char H_Data);
void I2C_LCD_Clear(void);
void I2C_LCD_Send_Char(char adress, char conv, char pnt);
void I2C_LCD_seg_conv(unsigned int h, unsigned int m, unsigned int s);
void I2C_LCD_print_time(void);
void I2C_LCD_set_print_time(char point);
void I2C_LCD_temp(unsigned int TM, unsigned int TL);
void I2C_LCD_print_temp(void);
#endif	/* MT107T_I2C_H */

