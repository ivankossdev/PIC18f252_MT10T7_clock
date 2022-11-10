#include <xc.h>
#include "set_1.h" 
#include "MT107T_I2C.h"

/****************PIC18F252***************
 * Connection PICkit4
 * pin 28 RB7/PGD  -> green
 * pin 27 RB6/PGC  -> white
 * pin 1  MCLR/VPP -> orange
 * pin 14 SCL
 * pin 15 SDA``
 ****************END**********************/

const int ds3231 = 0xD0;

unsigned int bin_To_Octal(unsigned int c) {
    return ((c / 10) << 4) | (c % 10);
}

unsigned int octal_To_Bin(unsigned int b) {
    return ((b & 0xf0) >> 4) * 10 + (b & 0x0f);
}

unsigned int setTime[3] = {
    33, 11, 14 
};

void SetTime(void) {
    unsigned int i;
    I2C_StartCondition();
    I2C_Write_Byte(ds3231);
    I2C_Write_Byte(0);
    while (BF);
    for (i = 0; i < 3; i++) {
        I2C_Write_Byte(bin_To_Octal(setTime[i]));
    }
    I2C_StopCondition();
}

void TIM1_init() {
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    TMR1CS = 0; //Internal clock
    TMR1L = 0xff; // 65536 - 31250 = 34286 = 0x85EE
    TMR1H = 0x7f;
    T0IE = 1;
    GIE = 1;
    PEIE = 1;
    TMR1IE = 1;
    TMR1ON = 1;
}

void interrupt tim_1(void) {

    if (TMR1IE && TMR1IF) {
        TMR1L = 0xff;
        TMR1H = 0x7f;
        TMR1IF = 0;
        PORTB ^= 0x10;
    }
}

void main(void) {
    PORTB = 0x00; //PB4 OUT
    TRISB = 0x00;
    InitI2C();
    TIM1_init();
    I2C_Send_Data(0x0f, 0x00, 0x01); //Unlock 
    I2C_LCD_Clear();
    //SetTime();
    while (1) {
        unsigned int i;
        unsigned int buf[3];

        I2C_StartCondition();
        I2C_Write_Byte(ds3231); 
        I2C_Write_Byte(0x00); 
        while (BF);
        I2C_Idle();
        SSPIF = 0;
        RSEN = 1;
        I2C_IntWait();
        I2C_Write_Byte(0xD0 | 0x01);
        for (i = 0; i < 3; i++) {
            if (i < (3 - 1)) {
                buf[i] = octal_To_Bin(I2C_ReceiveByte_Ack());
            } else {
                buf[i] = octal_To_Bin(I2C_ReceiveByte_Nack());
            }
        }
        I2C_StopCondition();

        I2C_LCD_seg_conv(buf[2], buf[1], buf[0]);
        I2C_LCD_print_time();
    }
}
