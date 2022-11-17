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
unsigned char flag_btn = 0;
unsigned int tim1Count = 0;

unsigned int dec_To_BinDec(unsigned int c) {
    return ((c / 10) << 4) | (c % 10);
}

unsigned int binDec_To_dec(unsigned int b) {
    return ((b & 0xf0) >> 4) * 10 + (b & 0x0f);
}

void TIM1_init(void) {
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

//void TIM0_init(void) {
//    GIE = 1;
//    TMR0ON = 1; // On
//    T08BIT = 1; // 8bit
//    T0CS = 0; //Internal instruction cycle clock
//    PSA = 0; //Prescaller On
//    // 1:256
//    T0PS2 = 1;
//    T0PS1 = 1;
//    T0PS0 = 1;
//    TMR0IE = 1;
//    TMR0IF = 0;
//    TMR0L = 0x00;
//}

void interrupt Timers(void) {
    if (TMR1IE && TMR1IF) {
        TMR1L = 0xff;
        TMR1H = 0x7f;
        TMR1IF = 0;
        if (RB0 == 1) {
            PORTB &= ~0x02;
            flag_btn = 1;
        } else if (RB2 == 1){
            flag_btn = 1;
        } else if (RB3 == 1){
            flag_btn = 1;
        } else {
            PORTB ^= 0x02;
            tim1Count++;
            if (tim1Count > 50){
                flag_btn = 1;
                tim1Count = 0;
            }
        }
    }
}

void getDataDs3231(unsigned char address) {
    unsigned char i;
    unsigned int buf[3];

    I2C_StartCondition();
    I2C_Write_Byte(ds3231);
    I2C_Write_Byte(address);
    while (BF);
    I2C_Idle();
    SSPIF = 0;
    RSEN = 1;
    I2C_IntWait();
    I2C_Write_Byte(0xD0 | 0x01);
    for (i = 0; i < 3; i++) {
        if (i < (3 - 1)) {
            buf[i] = binDec_To_dec(I2C_ReceiveByte_Ack());
        } else {
            buf[i] = binDec_To_dec(I2C_ReceiveByte_Nack());
        }
    }
    I2C_StopCondition();
    I2C_LCD_seg_conv(buf[2], buf[1], buf[0]);
    I2C_LCD_print_time();
}

void SetTime(void) {
    unsigned int setTime[3] = {
        33, 26, 8
    };
    unsigned int i;
    I2C_StartCondition();
    I2C_Write_Byte(ds3231);
    I2C_Write_Byte(0);
    while (BF);
    for (i = 0; i < 3; i++) {
        I2C_Write_Byte(dec_To_BinDec(setTime[i]));
    }
    I2C_StopCondition();
}

void SetData(void) {
    unsigned int setTime[3] = {
        14, 11, 22
    };
    unsigned int i;
    I2C_StartCondition();
    I2C_Write_Byte(ds3231);
    I2C_Write_Byte(0x04);
    while (BF);
    for (i = 0; i < 3; i++) {
        I2C_Write_Byte(dec_To_BinDec(setTime[i]));
    }
    I2C_StopCondition();
}

void main(void) {
    InitI2C();
    TIM1_init();
    I2C_Send_Data(0x0f, 0x00, 0x01); //Unlock 
    I2C_LCD_Clear();

    PORTB = 0x00; //PB4 OUT PB0 INPUT
    TRISB = 0x0D;
    //SetData();
    //SetTime();
    while (1) {
        if (flag_btn) {
            I2C_LCD_Clear();
            getDataDs3231(0x04);
            flag_btn = 0;
            __delay_ms(1000);
        }
        getDataDs3231(0x00);

    }
}
