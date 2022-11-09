#include "ÌÒ10Ò7.h"
/*********************** ÌÒ10Ò7*****************************
 *  -------------------------------------------------------
 * | 1 |  2 |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10 |
 *  -------------------------------------------------------
 * |E+ | V0 | GND | DB0 | DB1 | DB2 | DB3 | WR1 | WR2 | A0 |
 *  -------------------------------------------------------
 * |   |    |     | PÑ3 | PÑ4 | PÑ5 | PÑ6 | PÑ2 | PÑ1 | PÑ0|
 *  -------------------------------------------------------
 * *********************************************************   L    H
 * Ïîäêëþ÷åíèå äèñïëåÿ                                    0b ADEG FCBh                  
 * PÑ0-> A0              ****A****  DB0 G(L) DB0 h(H)   0 0b 1110 1110
 * PÑ1-> WR2             F       B  DB1 E(L) DB1 B(H)   1 0b 0000 0110
 * PÑ2-> WR1             *       *  DB2 D(L) DB2 C(H)   2 0b 1111 0010
 * PÑ3-> DB0             ****G****  DB3 A(L) DB3 F(H)   3 0b 1101 0110
 * PÑ4-> DB1             E       C                      4 0b 0001 1110
 * PÑ5-> DB2             *       *                      5 0b 1101 1100
 * PÑ6-> DB3             ****D**** h*                   6 0b 1111 1100
 *                                                      7 0b 1000 0110
 *                                                      8 0b 1111 1110
 *                                                      9 0b 1101 1110
 */
#define A0_ON     PORTC |= 0b00000001 
#define A0_OFF    PORTC &= 0b11111100 
#define WR1_ON    PORTC |= 0b00000100
#define WR1_OFF   PORTC &= 0b11111001
#define CLR_PORTC PORTC &= 0b00000111
char R1 = 0, R2 = 0, R3 = 0, R4 = 0;
char L_Half_Byte(char b) {
    char tmp;
    tmp = b & 0x0f;
    return tmp << 3;
}

void Send_Data(char adr, char L_Data, char H_Data) {
    A0_OFF;
    CLR_PORTC;
    PORTC |= L_Half_Byte(adr);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    CLR_PORTC;
    A0_ON;
    PORTC |= L_Half_Byte(L_Data);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    CLR_PORTC;
    PORTC |= L_Half_Byte(H_Data);
    WR1_ON;
    __delay_us(1);
    WR1_OFF;
    __delay_us(2);
}

char LCD_dig_conv(char digit) {
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

void LCD_Send_Char(char adress, char conv, char pnt) {
    char H_byte;
    char L_byte;
    char tmp = LCD_dig_conv(conv);
    if (pnt == 1) {
        tmp |= LCD_dig_conv('.');
    }
    L_byte = tmp & 0x0f;
    H_byte = tmp & 0xf0;
    H_byte = H_byte >> 4;
    Send_Data(adress, H_byte, L_byte);
}

void LCD_clear() {
    for (unsigned char i = 0; i <= 9; i++) {
        Send_Data(i, 0x00, 0x00);
    }
}

void LCD_seg_conv(unsigned int S) {
    R4 = S / 1000;
    R3 = S / 100 % 10;
    R2 = S / 10 % 10;
    R1 = S % 10;
}

void LCD_print_segment(unsigned int seg, unsigned char pos) {
    LCD_seg_conv(seg);
    LCD_Send_Char((pos + 3), R1, 0);
    LCD_Send_Char((pos + 2), R2, 0);
    LCD_Send_Char((pos + 1), R3, 0);
    LCD_Send_Char(pos, R4, 0);
}

void LCD_scroll(unsigned int seg, char pos) {
    LCD_seg_conv(seg);
    for (char i = pos; i <= 6; i++) {
        LCD_Send_Char((i + 3), R1, 0);
        LCD_Send_Char((i + 2), R2, 0);
        LCD_Send_Char((i + 1), R3, 0);
        LCD_Send_Char(i, R4, 0);
        __delay_ms(250);
        LCD_clear();
    }
    for (char i = 5; i > pos; i--) {
        LCD_Send_Char((i + 3), R1, 0);
        LCD_Send_Char((i + 2), R2, 0);
        LCD_Send_Char((i + 1), R3, 0);
        LCD_Send_Char(i, R4, 0);
        __delay_ms(250);
        LCD_clear();
    }
}

void LCD_Unlock() {
    Send_Data(0x0f, 0x00, 0x01);
    LCD_clear();
}
