    // Led de referênciia - Matriz
    

    #ifndef NPLEDS_H
    #define NPLEDS_H

    #include "pico/stdlib.h"
    #include "hardware/pio.h"
    #include "hardware/clocks.h"

    #include "ws2818b.pio.h" // Biblioteca gerada automaticamente

    #define LED_COUNT 25
    #define LED_PIN 7

    // Estrutura para representar um pixel RGB
    struct pixel_t {
        uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
    };
    typedef struct pixel_t pixel_t;
    typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

    // Declaração do buffer de pixels que formam a matriz.
    extern npLED_t leds[LED_COUNT];

    // Declaração das funções públicas
    void npInit(uint pin);
    void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b);
    void npClear();
    void npWrite();

    #endif 