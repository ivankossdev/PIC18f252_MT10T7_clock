#ifndef MT10T7_H
#include <xc.h>
#define _XTAL_FREQ 4000000

char L_Half_Byte(char b);
void Send_Data(char adr, char L_Data, char H_Data);
char LCD_dig_conv(char digit);
void LCD_Send_Char(char adress, char conv, char pnt);
void LCD_clear();
void LCD_seg_conv(unsigned int S);
void LCD_print_segment(unsigned int seg, unsigned char pos);
void LCD_scroll(unsigned int seg, char pos);
void LCD_Unlock();
#endif