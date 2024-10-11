#include "program.h"


// Delay x1.5us
void delay_x1o5us(uint8_t delay) {
    for(uint8_t i=0; i<delay; i++) NOP();
}

// Delay x24.25us
void delay_x24o25us(uint16_t delay) {
    for(uint16_t i=0; i<delay; i++) delay_x1o5us(15);
}

// Delay x1ms
void delay_ms(uint32_t delay) {
    for(uint32_t i=0; i<delay; i++) delay_x24o25us(41);
}


void programInitialize(void) {
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    
    led1 = 0;
    led2 = 0;
    
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB1 = 0;
    TRISCbits.TRISC5 = 0;
    
    ANSELBbits.ANSB5 = 0;
    ANSELBbits.ANSB4 = 0;
    ANSELBbits.ANSB3 = 0;
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB1 = 0;
    
    RS_Pin = 0;
    E_Pin = 0;
    D4_Pin = 0;
    D5_Pin = 0;
    D6_Pin = 0;
    D7_Pin = 0;
    
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    ANSELAbits.ANSA3 = 0;
    ANSELAbits.ANSA4 = 0;
    
    // Pin for servo PWM
    TRISBbits.TRISB0 = 0;
    ANSELBbits.ANSB0 = 0;
    
    lcd_Initialize();
    
    servo_MotorPosition(0);
}

void programLoop(void) {
    int16_t angle = 0;
    
    lcd_Goto(0, 0);
    lcd_PrintString("Servo Motor");
    
    lcd_Goto(1, 0);
    lcd_PrintDigitInt32(0, 3, false, true);
    
    while(1) {
        if(!pb_Left) {
            angle--;
            if(angle<0) angle = 180;
            
            lcd_Goto(1, 0);
            lcd_PrintDigitInt32(angle, 3, false, true);
            
            servo_MotorPosition((uint8_t)angle);
            
            pb_DelayDebounce();
        }
        
        if(!pb_Right) {
            angle++;
            if(angle>180) angle = 0;
            
            lcd_Goto(1, 0);
            lcd_PrintDigitInt32(angle, 3, false, true);
            
            servo_MotorPosition((uint8_t)angle);
            
            pb_DelayDebounce();
        }
        
        if(!pb_Up) {
            angle = 90;
            
            lcd_Goto(1, 0);
            lcd_PrintDigitInt32(angle, 3, false, true);
            
            servo_MotorPosition((uint8_t)angle);
            
            pb_DelayDebounce();
        }
        
        if(!pb_Down) {
            angle = 0;
            
            lcd_Goto(1, 0);
            lcd_PrintDigitInt32(angle, 3, false, true);
            
            servo_MotorPosition((uint8_t)angle);
            
            pb_DelayDebounce();
        }
    }
}


// Delay to debounce mechanical noise
void pb_DelayDebounce(void) {
    // delay_ms(100);
}

void lcd_DelaySetupTime(void) {
    // China TGK LCD delay
    delay_x1o5us(160);
}

void lcd_DelayPulse(void) {
    // China TGK LCD delay
    delay_x1o5us(160);
}

void lcd_EPulse(void) {
    E_Pin = 1;
    lcd_DelayPulse();
    
    E_Pin = 0;
    lcd_DelayPulse();
}

void lcd_WriteData(uint8_t data) {
    // Send upper nibble data
    D7_Pin = (data >> 7) & 0x01;
    D6_Pin = (data >> 6) & 0x01;
    D5_Pin = (data >> 5) & 0x01;
    D4_Pin = (data >> 4) & 0x01;
    
    lcd_EPulse();
    
    // Send lower nibble data
    D7_Pin = (data >> 3) & 0x01;
    D6_Pin = (data >> 2) & 0x01;
    D5_Pin = (data >> 1) & 0x01;
    D4_Pin = data & 0x01;
    
    lcd_EPulse();
    
    delay_x1o5us(25);   // Execution time for instruction >37us - Page 24
                        // delay = 37us / 1.5us = 25
}

void lcd_PrintCmd(uint8_t command) {
    RS_Pin = 0;
    lcd_DelaySetupTime();
    
    lcd_WriteData(command);
}

void lcd_Initialize(void) {
    delay_ms(10);           // Wait for LCD power supply rise time - Datasheet page 50
    
    // China TGK LCD reset process
    lcd_PrintCmd(0x33);
    lcd_PrintCmd(0x32);
    
    // LCD command - Datasheet page 24
    lcd_PrintCmd(0x28);     // Set LCD to 4-bit mode
    lcd_PrintCmd(0x02);     // Set DDRAM address counter to 0
    lcd_PrintCmd(0x0C);     // Display is set ON, cursor is set OFF, cursor blink is set OFF
    lcd_PrintCmd(0x06);     // Cursor is set to shift right
    lcd_PrintCmd(0x01);     // Clear entire display
    lcd_PrintCmd(0x80);     // Set cursor back to home
    
    delay_x24o25us(63);     // Execution time to clear display instruction, lcd_PrintCmd(0x01) >1.52ms,
                            // delay = 1.52ms / 24.25us = 63
}

void lcd_ClearAll(void) {
    lcd_PrintCmd(0x02);
    lcd_PrintCmd(0x01);
    delay_x24o25us(63);
}

void lcd_Goto(uint8_t y, uint8_t x) {
    switch(y) {
        case 0:
            lcd_PrintCmd(0x80 + x);
            break;
        
        case 1:
            lcd_PrintCmd(0xC0 + x);
            break;
        
        default:
            lcd_PrintCmd(0x80 + x);
            break;
    }
}

void lcd_PrintChar(char character) {
    RS_Pin = 1;
    lcd_DelaySetupTime();
    
    lcd_WriteData(character);
}

void lcd_PrintString(char *string) {
    while(*string!=0) {
        lcd_PrintChar(*string);
        string++;
    }
}

void lcd_PrintInt32(int32_t number) {
    uint8_t i1 = 0,
            i2 = 0,
            totalDigit = 0;
    
    char numberRevChar[11];
    char numberChar[11];
    
    memset(numberRevChar, 0, 11);
    memset(numberChar, 0, 11);
    
    if(number<0) {
        lcd_PrintChar('-');
        number = labs(number);
    }
    
    do {
        int32_t tempN = number;
        number /= 10;
        char tempC = (char)(tempN -10 * number);
        numberRevChar[i1] = tempC + 48;
        i1++;
    } while(number);
    
    totalDigit = i1;
    
    for(i1=totalDigit, i2=0; i1>0; i1--, i2++) {
        numberChar[i2] = numberRevChar[i1-1];
    }
    
    lcd_PrintString(numberChar);
}

void lcd_PrintDigitInt32(int32_t number, uint8_t noDigit, bool enSign, bool enZero) {
    uint8_t i1 = 0,
            i2 = 0,
            totalDigit = 0;
    
    char numberRevChar[11];
    char numberChar[11];
    
    memset(numberRevChar, 0, 11);
    memset(numberChar, 0, 11);
    
    if(number<0) {
        if(enSign) lcd_PrintChar('-');
        number = labs(number);
    } else {
        if(enSign) lcd_PrintChar(' ');
    }
    
    do {
        int32_t tempN = number;
        number /= 10;
        char tempC = (char)(tempN -10 * number);
        numberRevChar[i1] = tempC + 48;
        i1++;
    } while(number);
    
    totalDigit = i1;
    
    for(i1=0; i1<(noDigit-totalDigit); i1++) {
        if(enZero) lcd_PrintChar('0');
        else lcd_PrintChar(' ');
    }
    
    for(i1=totalDigit, i2=0; i1>0; i1--, i2++) {
        numberChar[i2] = numberRevChar[i1-1];
    }
    
    lcd_PrintString(numberChar);
}

void servo_MotorPosition(uint8_t angle) { // Generate 10 period PWM
    uint16_t i1 = 0,
             i2 = 0;
    
    uint16_t ton = (angle * 5) + 260;
    uint16_t toff = 11429 - ton;
    
    for(i1=0; i1<10; i1++) {
        servo_PWM = 1;
        for(i2=0; i2<ton; i2++) NOP();
        
        servo_PWM = 0;
        for(i2=0; i2<toff; i2++) NOP();
    }
}