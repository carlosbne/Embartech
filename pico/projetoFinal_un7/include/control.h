//Controle Joystick e botões

#ifndef CONTROL_H
#define CONTROL_H

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define BUTTON_A 5 
#define BUTTON_B 6 
#define VRY 27 // Pino do eixo Y do joystick

// Variáveis
extern volatile bool lock; // trava das cores
extern bool is_red;    // começa ajustando o vermelho
extern int intensity_red;  // intensidade atual da cor
extern int intensity_green; // intensidade atual da cor

//funções
void setup_adc();
void setup_buttons();
void button_callback(uint gpio, uint32_t events);


#endif