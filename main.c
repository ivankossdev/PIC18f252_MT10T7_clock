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
 *******************END******************/

const int ds3231 = 0xD0;
const char step_menu = 3;
unsigned char flag_tim1 = 0;
unsigned char flag_menu = 0;
unsigned char flag_clear = 1;
unsigned int tim1Count = 0;

unsigned int dec_To_BinDec(unsigned int c);
unsigned int binDec_To_dec(unsigned int b);
void TIM1_init(void);
void Int0Init(void);

void interrupt Timers(void) {
    if (TMR1IE && TMR1IF) {
        TMR1L = 0xff;
        TMR1H = 0x7f;
        TMR1IF = 0;
        PORTB ^= 0x02;
        tim1Count++;
        if (tim1Count > 50) {
            flag_tim1 = 1;
            tim1Count = 0;
        }
    } else if (INT0IE && INT0IF) {
        INT0IE = 0;
        TMR1IE = 0;
        flag_menu = 1;
        PORTB &= ~0x02;
    }
}

/*
 * Buttons
 * RB0
 * RB2
 * RB3
 */

unsigned char ButtonHandler(int but);
void getDataDs3231(unsigned char address);
void getTemperature(void);
void SetTime(void);
void SetData(void);

struct MenuPoints {
    unsigned char step;
    unsigned int setHr;
    unsigned int setMin;
    unsigned int setSec;
    unsigned int setDay;
    unsigned int setYear;
    unsigned int setMonth;
    unsigned int setDate;
    unsigned char sub_menu;
    unsigned char flag_confirmation;
};

struct MenuPoints d;

void SubMenuHandler(unsigned int *set0, unsigned int *set1, unsigned int *set2) {
    do {
        PORTB &= ~0x02;
        if (d.step == 0) {
            I2C_LCD_seg_conv(*set0, *set1, *set2);
            I2C_LCD_set_print_time(d.step);
            if (ButtonHandler(PORTBbits.RB2)) {
                (*set0)++;
                d.flag_confirmation = 1;
            } else if (ButtonHandler(PORTBbits.RB3)) {
                (*set0)--;
                d.flag_confirmation = 1;
            }
        }
        if (ButtonHandler(PORTBbits.RB0)) {
            d.step++;
        }
        if (d.step == 1) {
            I2C_LCD_seg_conv(*set0, *set1, *set2);
            I2C_LCD_set_print_time(d.step);
            if (ButtonHandler(PORTBbits.RB2)) {
                (*set1)++;
                d.flag_confirmation = 1;
            } else if (ButtonHandler(PORTBbits.RB3)) {
                (*set1)--;
                d.flag_confirmation = 1;
            }
        }
        if (d.step == 2) {
            I2C_LCD_seg_conv(*set0, *set1, *set2);
            I2C_LCD_set_print_time(d.step);
            if (ButtonHandler(PORTBbits.RB2)) {
                (*set2)++;
                d.flag_confirmation = 1;
            } else if (ButtonHandler(PORTBbits.RB3)) {
                (*set2)--;
                d.flag_confirmation = 1;
            }
        }
        if (ButtonHandler(PORTBbits.RB0)) {
            d.step++;
        }
    } while (d.step <= 2);
}

void SubMenu(unsigned char _sub_menu, unsigned char menu) {
    d.step = 0;
    d.flag_confirmation = 0;
    switch (_sub_menu) {
        case 0:
            I2C_LCD_Clear();
            I2C_LCD_Send_Char(9, menu, 0);
            getTemperature();
            break;
        case 1:
            I2C_LCD_Clear();
            I2C_LCD_Send_Char(9, menu, 0);
            SubMenuHandler(&d.setHr, &d.setMin, &d.setSec);
            if (d.flag_confirmation) {
                SetTime();
            }
            break;
        case 2:
            I2C_LCD_Clear();
            I2C_LCD_Send_Char(9, menu, 0);
            SubMenuHandler(&d.setYear, &d.setMonth, &d.setDate);
            if (d.flag_confirmation) {
                SetData();
            }
            break;
    }
}

void Menu(unsigned char menu) {
    switch (menu) {
        case 0: SubMenu(d.sub_menu, menu);
            break;
        case 1: SubMenu(d.sub_menu, menu);
            break;
        case 2: SubMenu(d.sub_menu, menu);
            break;
    }
    d.sub_menu++;
    if (d.sub_menu == step_menu) d.sub_menu = 0;
}

void main(void) {
    InitI2C();
    Int0Init();
    TIM1_init();
    I2C_Send_Data(0x0f, 0x00, 0x01); //Unlock 
    I2C_LCD_Clear();

    PORTB = 0x00; //PB4 OUT PB0 INPUT
    TRISB = 0x0D;
    unsigned char menu = 0;

    while (1) {
        if (flag_tim1) { //Показывает дату по прерыванию 
            I2C_LCD_Clear(); //таймера tim1 
            getDataDs3231(0x04);
            flag_tim1 = 0;
            __delay_ms(1000);
            I2C_LCD_Clear();
        } else if (flag_menu) { //Точка входа в меню настройки
            if (flag_clear) {
                I2C_LCD_Clear();
                flag_clear = 0;
            }
            if (ButtonHandler(PORTBbits.RB0)) {
                Menu(menu);
                PORTB |= 0x02;
                menu++;
                if (menu == step_menu) menu = 0;
            } else if (ButtonHandler(PORTBbits.RB3)) {
                TMR1IE = 1;
                TMR1IF = 0;
                INT0IE = 1;
                INT0IF = 0;
                flag_menu = 0;
                flag_clear = 1;
                menu = 0;
                d.sub_menu = 0;
                tim1Count = 0;
                PORTB &= ~0x02;
                I2C_LCD_Clear();
            }
        } else {
            getDataDs3231(0x00);
        }
    }
}

unsigned int dec_To_BinDec(unsigned int c) {
    return ((c / 10) << 4) | (c % 10);
}

unsigned int binDec_To_dec(unsigned int b) {
    return ((b & 0xf0) >> 4) * 10 + (b & 0x0f);
}

unsigned char ButtonHandler(int but) {
    unsigned char result = 0;
    unsigned int butCount = 0;
    while (but) {
        if (butCount < 10000) {
            butCount++;
        } else {
            result = 1;
            break;
        }
    }
    return result;
}

void Int0Init(void) {
    INT0IE = 1;
    INT0IF = 0;
    INTEDG0 = 1;
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

    if (address == 0x04) {
        d.setYear = buf[2];
        d.setMonth = buf[1];
        d.setDate = buf[0];
    } else {
        d.setHr = buf[2];
        d.setMin = buf[1];
        d.setSec = buf[0];
    }

    I2C_LCD_seg_conv(buf[2], buf[1], buf[0]);
    I2C_LCD_print_time();
}

void getTemperature(void) {
    unsigned int buf[2];

    I2C_StartCondition();
    I2C_Write_Byte(ds3231);
    I2C_Write_Byte(0x11);
    while (BF);
    I2C_Idle();
    SSPIF = 0;
    RSEN = 1;
    I2C_IntWait();
    I2C_Write_Byte(0xD0 | 0x01);
    buf[0] = I2C_ReceiveByte_Ack();
    buf[1] = I2C_ReceiveByte_Nack();
    I2C_StopCondition();
    I2C_LCD_temp(buf[0], (buf[1] >> 6) * 25);
    I2C_LCD_print_temp();
}

void SetTime(void) {
    unsigned int setTime[3];
    setTime[0] = d.setSec;
    setTime[1] = d.setMin;
    setTime[2] = d.setHr;
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
    unsigned int setTime[3];
    setTime[0] = d.setDate;
    setTime[1] = d.setMonth;
    setTime[2] = d.setYear;
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
