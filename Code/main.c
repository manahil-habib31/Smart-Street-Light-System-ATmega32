/*
 * ============================================================
 *  Smart Street Light System — WITH LCD + BUZZER
 *  MCU  : ATmega32 @ 16 MHz
 *  Tool : Microchip Studio 7 + SimulIDE 1.1.0
 * ============================================================
 *
 *  PIN CONNECTIONS:
 *  PA0 (pin 40) — LDR voltage divider
 *  PD2 (pin 16) — Push button (PIR simulation)
 *  PD5 (pin 19) — LED via 330 ohm resistor
 *  PC0 (pin 22) — Buzzer
 *  PC1 (pin 23) — LCD RS
 *  PC2 (pin 24) — LCD EN
 *  PC3 (pin 25) — LCD D4
 *  PC4 (pin 26) — LCD D5
 *  PC5 (pin 27) — LCD D6
 *  PC6 (pin 28) — LCD D7
 * ============================================================
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

/* ── CONFIG ─────────────────────────────────── */
#define DAY_THRESHOLD   512
#define MOTION_TIMEOUT   30

/* ── LCD PIN DEFINES ────────────────────────── */
#define LCD_RS   PC1
#define LCD_EN   PC2
#define LCD_D4   PC3
#define LCD_D5   PC4
#define LCD_D6   PC5
#define LCD_D7   PC6

/* ── GLOBALS ────────────────────────────────── */
volatile uint8_t  motion_detected = 0;
volatile uint16_t timeout_counter = 0;

/* ══════════════════════════════════════════════
   LCD FUNCTIONS (4-bit mode)
══════════════════════════════════════════════ */

void LCD_Enable(void)
{
    PORTC |=  (1 << LCD_EN);
    _delay_us(1);
    PORTC &= ~(1 << LCD_EN);
    _delay_us(50);
}

/* Send a nibble (4 bits) to LCD data pins D4-D7 */
void LCD_Nibble(uint8_t nibble)
{
    /* Clear D4-D7 bits (PC3-PC6) then set them */
    PORTC = (PORTC & 0x87) | ((nibble & 0x0F) << 3);
    LCD_Enable();
}

/* Send full byte — high nibble first, then low nibble */
void LCD_Send(uint8_t data, uint8_t rs)
{
    if (rs)
        PORTC |=  (1 << LCD_RS);   /* RS=1: data */
    else
        PORTC &= ~(1 << LCD_RS);   /* RS=0: command */

    LCD_Nibble(data >> 4);          /* High nibble */
    LCD_Nibble(data & 0x0F);        /* Low nibble  */

    if (data == 0x01 || data == 0x02)
        _delay_ms(2);               /* Clear/home commands need extra time */
    else
        _delay_us(100);
}

void LCD_Command(uint8_t cmd)  { LCD_Send(cmd, 0); }
void LCD_Char(char c)          { LCD_Send(c,   1); }

void LCD_String(const char *s)
{
    while (*s)
        LCD_Char(*s++);
}

/* Set cursor: row 0 or 1, col 0-15 */
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x80 : 0xC0;
    LCD_Command(addr + col);
}

/* Print a string padded to 16 chars (clears old text) */
void LCD_Print16(uint8_t row, const char *s)
{
    LCD_SetCursor(row, 0);
    uint8_t len = strlen(s);
    for (uint8_t i = 0; i < 16; i++)
    {
        if (i < len)
            LCD_Char(s[i]);
        else
            LCD_Char(' ');   /* pad with spaces */
    }
}

void LCD_Init(void)
{
    /* PC0-PC6 all as output */
    DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5)|(1<<PC6);
    PORTC = 0x00;

    _delay_ms(50);          /* Wait for LCD power-up */

    /* Initialization sequence for 4-bit mode */
    PORTC &= ~(1 << LCD_RS);

    LCD_Nibble(0x03); _delay_ms(5);
    LCD_Nibble(0x03); _delay_ms(1);
    LCD_Nibble(0x03); _delay_ms(1);
    LCD_Nibble(0x02); _delay_ms(1);   /* Set to 4-bit mode */

    LCD_Command(0x28);   /* 4-bit, 2 lines, 5x8 font */
    LCD_Command(0x0C);   /* Display ON, cursor OFF    */
    LCD_Command(0x06);   /* Entry mode: increment     */
    LCD_Command(0x01);   /* Clear display             */
    _delay_ms(2);
}

/* ══════════════════════════════════════════════
   ADC
══════════════════════════════════════════════ */
void ADC_Init(void)
{
    ADMUX  = (1 << REFS0);
    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint16_t ADC_Read(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

/* ══════════════════════════════════════════════
   TIMER0 — 10ms tick for 30sec timeout
══════════════════════════════════════════════ */
void Timer0_Init(void)
{
    TCCR0  = (1 << WGM01) | (1 << CS02) | (1 << CS00);
    OCR0   = 155;
    TIMSK |= (1 << OCIE0);
}

/* ══════════════════════════════════════════════
   INT0 — button / PIR on PD2
══════════════════════════════════════════════ */
void INT0_Init(void)
{
    DDRD  &= ~(1 << PD2);
    MCUCR |=  (1 << ISC01) | (1 << ISC00);
    GICR  |=  (1 << INT0);
}

/* ══════════════════════════════════════════════
   LED CONTROL on PD5
══════════════════════════════════════════════ */
void LED_OFF(void)  { PORTD &= ~(1 << PD5); }
void LED_FULL(void) { PORTD |=  (1 << PD5); }
void LED_DIM(void)
{
    PORTD |=  (1 << PD5); _delay_ms(80);
    PORTD &= ~(1 << PD5); _delay_ms(80);
}

/* ══════════════════════════════════════════════
   BUZZER — short beep on PC0
══════════════════════════════════════════════ */
void Buzzer_Beep(void)
{
    PORTC |=  (1 << PC0);
    _delay_ms(150);
    PORTC &= ~(1 << PC0);
}

/* ══════════════════════════════════════════════
   ISRs
══════════════════════════════════════════════ */
ISR(INT0_vect)
{
    motion_detected = 1;
    timeout_counter = 0;
}

ISR(TIMER0_COMP_vect)
{
    if (motion_detected)
    {
        timeout_counter++;
        if (timeout_counter >= (uint16_t)(MOTION_TIMEOUT * 100UL))
        {
            motion_detected = 0;
            timeout_counter = 0;
        }
    }
}

/* ══════════════════════════════════════════════
   MAIN
══════════════════════════════════════════════ */
int main(void)
{
    /* PD5 as output — LED */
    DDRD |= (1 << PD5);

    /* Initialize all peripherals */
    ADC_Init();
    Timer0_Init();
    INT0_Init();
    LCD_Init();
    sei();

    /* Show startup message */
    LCD_Print16(0, "Smart StreetLight");
    LCD_Print16(1, "Initializing...");
    _delay_ms(1500);
    LCD_Command(0x01);   /* Clear LCD */
    _delay_ms(2);

    uint16_t ldr          = 0;
    uint8_t  prev_state   = 255;   /* Track previous state to avoid redrawing LCD every loop */
    uint8_t  curr_state   = 0;
    /* States: 0 = DAY, 1 = NIGHT-DIM, 2 = NIGHT-MOTION */

    while (1)
    {
        ldr = ADC_Read();

        if (ldr >= DAY_THRESHOLD)
        {
            /* ── DAY MODE ── */
            curr_state      = 0;
            motion_detected = 0;
            timeout_counter = 0;
            LED_OFF();
            PORTC &= ~(1 << PC0);   /* Buzzer OFF */

            if (curr_state != prev_state)
            {
                LCD_Print16(0, "  MODE: DAY     ");
                LCD_Print16(1, "  LED: OFF      ");
                prev_state = curr_state;
            }

            _delay_ms(100);
        }
        else
        {
            /* ── NIGHT MODE ── */
            if (motion_detected)
            {
                /* NIGHT + MOTION */
                curr_state = 2;
                LED_FULL();

                if (curr_state != prev_state)
                {
                    LCD_Print16(0, "  MODE: NIGHT   ");
                    LCD_Print16(1, "  MOTION DETECT!");
                    Buzzer_Beep();   /* Single beep on motion */
                    prev_state = curr_state;
                }

                _delay_ms(100);
            }
            else
            {
                /* NIGHT + NO MOTION */
                curr_state = 1;
                PORTC &= ~(1 << PC0);   /* Buzzer OFF */

                if (curr_state != prev_state)
                {
                    LCD_Print16(0, "  MODE: NIGHT   ");
                    LCD_Print16(1, "  LED: DIM      ");
                    prev_state = curr_state;
                }

                LED_DIM();   /* This takes ~160ms per call */
            }
        }
    }

    return 0;
}