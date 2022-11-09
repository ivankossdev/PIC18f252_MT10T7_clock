#include "MT107T_I2C.h"
/*********************** МТ10Т7*****************************
 *  -------------------------------------------------------
 * | 1 |  2 |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10 |
 *  -------------------------------------------------------
 * |E+ | V0 | GND | DB0 | DB1 | DB2 | DB3 | WR1 | WR2 | A0 |
 *  -------------------------------------------------------
 * |   |    |     | PС3 | PС4 | PС5 | PС6 | PС2 | PС1 | PС0|
 *  -------------------------------------------------------
 * *********************************************************   L    H
 * Подключение дисплея                                   0b ADEG FCBh                  
 * P0-> A0              ****A****  DB0 G(L) DB0 h(H)   0 0b 1110 1110
 * P1-> WR2             F       B  DB1 E(L) DB1 B(H)   1 0b 0000 0110
 * P2-> WR1             *       *  DB2 D(L) DB2 C(H)   2 0b 1111 0010
 * P3-> DB0             ****G****  DB3 A(L) DB3 F(H)   3 0b 1101 0110
 * P4-> DB1             E       C                      4 0b 0001 1110
 * P5-> DB2             *       *                      5 0b 1101 1100
 * P6-> DB3             ****D**** h*                   6 0b 1111 1100
 *                                                     7 0b 1000 0110
 *                                                     8 0b 1111 1110
 *                                                     9 0b 1101 1110
 */

char buffer = 0;

struct Segment {
    char S0;
    char S1;
    char S2;
    char S3;
    char S4;
    char S5;
    char S6;
    char S7;
    char S8;
    char S9;
};

char Half_Byte(char b) {
    char tmp;
    tmp = b & 0x0f;
    return tmp << 3;
}

void I2C_Send_Data(char adr, char L_Data, char H_Data) {
    A0_OFF;
    CLR_buffer;
    buffer |= Half_Byte(adr);
    I2C_SendByteByADDR(buffer, 0x70);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    CLR_buffer;
    A0_ON;
    buffer |= Half_Byte(L_Data);
    I2C_SendByteByADDR(buffer, 0x70);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    CLR_buffer;
    buffer |= Half_Byte(H_Data);
    I2C_SendByteByADDR(buffer, 0x70);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    __delay_us(2);
}

void I2C_LCD_Clear(void) {
    for (unsigned char i = 0; i <= 9; i++) {
        I2C_Send_Data(i, 0x00, 0x00);
    }
}

char I2C_LCD_dig_conv(char digit) {
    char d;
    switch (digit) {
        case 0: d = 0b11101110;
            break;
        case 1: d = 0b00000110;
            break;
        case 2: d = 0b11110010;
            break;
        case 3: d = 0b11010110;
            break;
        case 4: d = 0b00011110;
            break;
        case 5: d = 0b11011100;
            break;
        case 6: d = 0b11111100;
            break;
        case 7: d = 0b10000110;
            break;
        case 8: d = 0b11111110;
            break;
        case 9: d = 0b11011110;
            break;
        case '.': d = 0b00000001;
            break;
        case '-': d = 0b00010000;
            break;
    }
    return d;
}

void I2C_LCD_Send_Char(char adress, char conv, char pnt) {
    char H_byte;
    char L_byte;
    char tmp = I2C_LCD_dig_conv(conv);
    if (pnt == 1) {
        tmp |= I2C_LCD_dig_conv('.');
    }
    L_byte = tmp & 0x0f;
    H_byte = tmp & 0xf0;
    H_byte = H_byte >> 4;
    I2C_Send_Data(adress, H_byte, L_byte);
}

struct Segment segment;

void I2C_LCD_seg_conv(unsigned int h, unsigned int m, unsigned int s) {
    segment.S1 = h / 10 % 10;
    segment.S0 = h % 10;
    segment.S3 = m / 10 % 10;
    segment.S2 = m % 10;
    segment.S5 = s / 10 % 10;
    segment.S4 = s % 10;
}

void I2C_LCD_print_time(void) {
    
    I2C_LCD_Send_Char(0, segment.S1, 0);
    I2C_LCD_Send_Char(1, segment.S0, 0);
    I2C_LCD_Send_Char(2, '-', 0);
    I2C_LCD_Send_Char(3, segment.S3, 0);
    I2C_LCD_Send_Char(4, segment.S2, 0);
    I2C_LCD_Send_Char(5, '-', 0);
    I2C_LCD_Send_Char(6, segment.S5, 0);
    I2C_LCD_Send_Char(7, segment.S4, 0);
}