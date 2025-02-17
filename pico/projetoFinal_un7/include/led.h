// LED Ajustavél do usuário
#ifndef LED_H
#define LED_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"


//pinos dos LED
#define LED_RED 13
#define LED_GREEN 11

// Funções LED do Usuário
void setup_pwm();
void adjust_color();

#endif