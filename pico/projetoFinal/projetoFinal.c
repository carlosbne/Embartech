#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
/* #include "hardware/dma.h" */
#include "ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
/* #include "hardware/timer.h" */
#include "hardware/clocks.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

//Definição dos Pinos
/* #define I2C_PORT i2c1  //pinos e módulos controlador i2c selecionado
#define PINO_SCL 14  
#define PINO_SDA 15
#define BUZZER_PIN 21  // Define o pino do buzzer */
#define SW 22          // Botão do Joystick
#define VRX 26         // Pino ADC do eixo X do joystick
#define VRY 27         // Pino ADC do eixo Y do joystick

// Definição do número de LEDs e pino
#define LED_COUNT 25
#define LED_PIN 7

const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y

//definição dos LEDs RGB
const uint BLUE_LED_PIN= 12;   // LED azul no GPIO 12
const uint RED_LED_PIN  = 13; // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11;  // LED verde no GPIO 11

// Define os slices PWM e canais para cada cor
#define LED_VERMELHO_SLICE pwm_gpio_to_slice_num(RED_LED_PIN)
#define LED_VERMELHO_CANAL pwm_gpio_to_channel(RED_LED_PIN)

#define LED_VERDE_SLICE pwm_gpio_to_slice_num(GREEN_LED_PIN)
#define LED_VERDE_CANAL pwm_gpio_to_channel(GREEN_LED_PIN)

#define LED_AZUL_SLICE pwm_gpio_to_slice_num(BLUE_LED_PIN)
#define LED_AZUL_CANAL pwm_gpio_to_channel(BLUE_LED_PIN)

//parte do código retirado de https://github.com/BitDogLab/BitDogLab-C/tree/main/neopixel_pio
// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits, cada um, compõem um pixel.
};

typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t", por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;
float brightness = 1.0; // Variável de controle de brilho (0.0 a 1.0)

/**
* Inicializa a máquina PIO para controle da matriz de LEDs.
*/
void npInit(uint pin) {
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }
    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
    }
}

/**
 * Define o brilho da matriz sem alterar as cores.
 */
void SetBrightness(float b) {
    if (b < 0.0) b = 0.0;
    if (b > 1.0) b = 1.0;
    brightness = b;
}

/**
* Atribui uma cor RGB a um LED.
*/
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = (uint8_t)(r * brightness);
    leds[index].G = (uint8_t)(g * brightness);
    leds[index].B = (uint8_t)(b * brightness);
   }


/**
* Escreve os dados do buffer nos LEDs.
*/
void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        uint8_t r = (uint8_t)(leds[i].R * brightness);
        uint8_t g = (uint8_t)(leds[i].G * brightness);
        uint8_t b = (uint8_t)(leds[i].B * brightness);
        pio_sm_put_blocking(np_pio, sm, g);
        pio_sm_put_blocking(np_pio, sm, r);
        pio_sm_put_blocking(np_pio, sm, b);
    }
}
/**
* Limpa o buffer de pixels.
*/
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
      npSetLED(i, 0, 0, 0);
   }
   

void setupLEDs() {
    SetBrightness(0.1); // Ajusta o brilho para 50%
    npInit(LED_PIN);
    npClear();
    npSetLED(22, 255, 255, 0);
    npWrite();
}

// ------------- Configutações PWM -------------
const float DIVIDER_PWM = 16.0; // divisor de clock para o PWM
const uint16_t PERIOD_PWM = 65535;   // período do PWM (16 bits)

//ssd1306_t disp; 
//volatile bool J_EXIT = false;  // variável para indicar saída da função

/*  //config do DMA que já veio no arquivo ao criar
// Data will be copied from src to dst
const char src[] = "Hello, world! (from DMA)";
char dst[count_of(src)]; */


void inicializa(){
    stdio_init_all();
    stdio_usb_init();
    //inicializa ADC 
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);
    //inicializa I2C
/*     i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT); */

    //inicialização dos LEDs
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialmente, desligar o LED RGB
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    //botão do Joystick
    gpio_init(SW);  
    gpio_set_dir(SW, GPIO_IN); // config o pino do botão como entrada
    gpio_pull_up(SW);
}


//-------------- Função -------------------
void setup_pwm_LED()
{
    //pinos GPIO para PWM
    gpio_set_function(RED_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(GREEN_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(BLUE_LED_PIN, GPIO_FUNC_PWM);

    // wrap value para todos os slices (igual para RGB neste caso)
    pwm_set_wrap(LED_VERMELHO_SLICE, PERIOD_PWM);
    pwm_set_wrap(LED_VERDE_SLICE, PERIOD_PWM);
    pwm_set_wrap(LED_AZUL_SLICE, PERIOD_PWM);

    //definição do divisor de clock para todos os slices (igual para RGB neste caso)
    pwm_set_clkdiv(LED_VERMELHO_SLICE, DIVIDER_PWM);
    pwm_set_clkdiv(LED_VERDE_SLICE, DIVIDER_PWM);
    pwm_set_clkdiv(LED_AZUL_SLICE, DIVIDER_PWM);

    //inicializa os LEDs com duty cycle 0 (desligados)
    pwm_set_chan_level(LED_VERMELHO_SLICE, LED_VERMELHO_CANAL, 0);
    pwm_set_chan_level(LED_VERDE_SLICE, LED_VERDE_CANAL, 0);
    pwm_set_chan_level(LED_AZUL_SLICE, LED_AZUL_CANAL, 0);

    //PWM para cada slice
    pwm_set_enabled(LED_VERMELHO_SLICE, true);
    pwm_set_enabled(LED_VERDE_SLICE, true);
    pwm_set_enabled(LED_AZUL_SLICE, true);

}


// Função para definir a cor do LED RGB
void led(uint8_t r, uint8_t g, uint8_t b) {
    // Converte os valores de 0-255 para 0-65535
    uint16_t duty_r = (uint16_t)(r * 65535 / 255);
    uint16_t duty_g = (uint16_t)(g * 65535 / 255);
    uint16_t duty_b = (uint16_t)(b * 65535 / 255);

    // Definir o nível do duty cycle para cada canal PWM, 
    // controlando assim o brilho de cada componente do LED RGB.
    pwm_set_chan_level(LED_VERMELHO_SLICE, LED_VERMELHO_CANAL, duty_r);
    pwm_set_chan_level(LED_VERDE_SLICE, LED_VERDE_CANAL, duty_g);
    pwm_set_chan_level(LED_AZUL_SLICE, LED_AZUL_CANAL, duty_b);
}



int main()
{
    inicializa();
    setup_pwm_LED();
    // Inicializa matriz de LEDs
    setupLEDs();
    
    // Testar a saída
    printf("Programa LED RGB e Joystick para Raspberry Pi Pico W em C\n");

    while(1){
        
        // leitura dos valores analógicos do joystick
        adc_select_input(ADC_CHANNEL_0);
        uint16_t adc_x_raw = adc_read();

        adc_select_input(ADC_CHANNEL_1);
        uint16_t adc_y_raw = adc_read();

        // botão do joystick 
        bool button_state = !gpio_get(SW); // Botão é normalmente pull-up, então !gpio_get() inverte para lógica ativa alta

        // Normaliza os valores do joystick para o intervalo 0-255 (para controle do LED RGB)
        uint8_t led_r_val = adc_x_raw * 255 / 4095; // ADC é de 12 bits (0-4095)
        uint8_t led_g_val = adc_y_raw * 255 / 4095;
        uint8_t led_b_val = button_state ? 255 : 0; // Azul acende se o botão for pressionado

        // Inverte as cores Vermelho e Verde para ter um controle mais intuitivo com o Joystick (opcional)
        led(led_r_val, led_g_val, led_b_val);

        printf("Joystick X: %u, Y: %u, Button: %s, RGB: (%u, %u, %u)\n",
               adc_x_raw, adc_y_raw, button_state ? "PRESSED" : "RELEASED",
               led_r_val, led_g_val, led_b_val);

        sleep_ms(100); // Pequeno delay para evitar leituras muito rápidas
    }

    return 0;

}