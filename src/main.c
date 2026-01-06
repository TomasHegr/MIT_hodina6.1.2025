#include <stdbool.h>
#include <stm8s.h>
// #include <stdio.h>
#include "daughterboard.h"
#include "main.h"
#include "milis.h"

/* GLOBÁLNÍ PROMĚNNÉ JSOU DOSTUPNÉ V CELÉM MODULU main.c */
#define LENGHT 5
uint32_t led_speed = 100;
uint32_t btn_press_history[LENGHT]; // Pole pro ukládání časů
uint8_t history_index = 0;          // index pro pole btn_press_history


void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    init_milis();
    // init_uart1();

    GPIO_Init(LED6_PORT, LED6_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(S1_PORT, S1_PIN, GPIO_MODE_IN_PU_NO_IT);
}
void btn_press_handler(uint32_t time)
{

    uint32_t suma = 0;
    // history_index %=LENGHT; //jednoduší zápis (zbytek po dělení) -
    // history_index cykluje 0 až LENGHT
    
    if (history_index >= LENGHT) { // složitější zápis
        history_index = 0;
    }
    btn_press_history[history_index] = time;
    history_index++;

    //první tap vstoupí do režimu programování a poslední ho ukončí
    if (history_index == LENGHT) {

        for (uint8_t i = 0; i < LENGHT; i++) {
            suma += btn_press_history[i];
        }
        led_speed = suma / LENGHT;
    } else {
        led_speed = 10; // LED svítí jako indikace toho, že probíhá programování
    }
}

int main(void)
{

    uint32_t led_time = 0;
    uint32_t btn_time = 0;
    uint32_t raise_time, fall_time;
    bool btn_is_pressed = false;

    init();

    while (1) {
        // blikání LED
        if (milis() - led_time > led_speed) {
            led_time = milis();
            REVERSE(LED6);
        }
        // kontrola tlačítka každých 10ms
        if (milis() - btn_time > 10) {
            btn_time = milis();

            // náběžná hrana
            if (PUSH(S1) && !btn_is_pressed) {
                raise_time = btn_time; // uložím si čas
            }

            // sestupná hrana
            if (!PUSH(S1) && btn_is_pressed) {
                fall_time = btn_time; // uložím si čas
                btn_press_handler(fall_time - raise_time);
            }

            btn_is_pressed = PUSH(S1);
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
