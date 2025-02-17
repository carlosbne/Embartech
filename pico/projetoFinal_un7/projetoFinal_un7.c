#include <stdio.h>
#include <stdlib.h>
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "led.h"
#include "control.h"
#include "npLeds.h"
/* #include "wifi.h" */


// Variáveis estado
volatile bool lock = false; // trava das cores
bool is_red = true;    // começa ajustando o vermelho
int intensity_red = 76;  // 30% de 255
int intensity_green = 76; // 30% de 255
int color = 0;
int margin = 30; // margem de erro

int reference_red = 39;  //vermelho refência
int reference_green = 38; //verde referência

// Variáveis para debounce
uint64_t last_button_a_time = 0; // Último tempo em que o botão A foi pressionado
uint64_t last_button_b_time = 0; // Último tempo em que o botão B foi pressionado
uint32_t debounce_interval = 200000; // = 200 ms 

// Variáveis Matriz
npLED_t leds[LED_COUNT]; // Buffer de pixels
PIO np_pio;              // Máquina PIO
uint sm;                 // Estado da máquina

typedef struct {
    int red;
    int green;
} Colors;


// Função para calcular o nível de daltonismo
int calculate_colorblindness(int ref_red, int ref_green, int user_red, int user_green) {
    int diff_red = abs(ref_red - user_red);
    int diff_green = abs(ref_green - user_green);
    int colorblind_level = (diff_red + diff_green) / 2;

    //nível de daltonismo
    if(colorblind_level <= 20){
        printf("Visão normal\n");
    } else if (colorblind_level <= 70){
        printf("Daltonismo Moderado\n");
     
    } else {
        printf("Daltonismo Severo\n");
        
    }
    //Diferencia o tipo de daltonismo 
    if(diff_green > margin){
        printf("Deuteranopia (dificuldade ou incapacidade de perceber o verde)\n");
    } 
    if(diff_red > margin){
        printf("Drotanopia (dificuldade ou incapacidade de perceber o vermelho)\n");
    }

    printf("Nível atual = %d %% \n", colorblind_level);
    return colorblind_level;
}


// função de interrupção para travar as cores
void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_B) {
        uint64_t current_time2 = time_us_64();
        if(current_time2 - last_button_b_time >= debounce_interval){
            lock = !lock; //trava valores atuais 
            if(lock){
                printf("Travado, Resultado = Vermelho: %d, Verde: %d\n", intensity_red, intensity_green);
                calculate_colorblindness(reference_red, reference_green, intensity_red, intensity_green);
            }else{
                printf("Destravado, ajuste novamente");
        }
            last_button_b_time = current_time2; // Reinicia o temporizador
        }
    }
}


// função para configurar o PWM dos LEDs
void setup_pwm() {
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_GREEN, GPIO_FUNC_PWM);

    uint slice_num_red = pwm_gpio_to_slice_num(LED_RED);
    pwm_set_wrap(slice_num_red, 255);  // Definir o limite de PWM para o LED vermelho
    pwm_set_enabled(slice_num_red, true);

    uint slice_num_green = pwm_gpio_to_slice_num(LED_GREEN);
    pwm_set_wrap(slice_num_green, 255);  // Definir o limite de PWM para o LED verde
    pwm_set_enabled(slice_num_green, true);
}

// função para config adc
void setup_adc() {
    adc_gpio_init(VRY);  // ativa como entrada ADC
    adc_select_input(1); // seleciona o canal 1 do ADC
}

// função para ajustar a cor mantendo os valores anteriores
void adjust_color() {
    pwm_set_gpio_level(LED_RED, intensity_red);
    pwm_set_gpio_level(LED_GREEN, intensity_green);
}

////-------- Matriz ------------
//Código aproveitado de https://github.com/BitDogLab/BitDogLab-C/blob/main/neopixel_pio/Readme.md
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
    npClear();
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    if (index < LED_COUNT) {
        leds[index].R = r;
        leds[index].G = g;
        leds[index].B = b;
    }
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}
//-------- Fim Matriz ------------

//------- Conexão Wi-fi Com ThingSpeak ---------


// função de inicialização
void init(){
    stdio_init_all();
    setup_pwm(); 
    adc_init();

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(VRY);
    gpio_set_dir(VRY, GPIO_IN);
    gpio_pull_up(VRY);

    npInit(LED_PIN);
}

// função principal
int main() {
    stdio_init_all();
    init();
    
    // Configura um LED Matriz (led de referência)
    npClear();
    npSetLED(17, reference_red, reference_green, 0);
    npWrite(); // Atualiza os LEDs
    
    // definição temporizador
    uint64_t start_time = time_us_64();  // recebe tempo inicial
    uint64_t last_button_a_time = 0;    // Tempo inicial para o debounce do botão A

    while (true) {
        if(time_us_64() - start_time >= 200000){
            //efetua a cada 200ms
            if(!lock){
                //ler joystick e atualiza valor na cor selecionada
                uint16_t adc_y_raw = adc_read();
                int adjustment = 0; //padrão
        
                //espaço para leitura
                if(adc_y_raw < 2000 || adc_y_raw > 2096){
                    adjustment = (adc_y_raw - 2048) / 128; //att em +16 / -16
        
                    if (is_red) {
                        intensity_red += adjustment;
                        if (intensity_red > 255) intensity_red = 255;
                        if (intensity_red < 0) intensity_red = 0;
                    } else {
                        intensity_green += adjustment;
                        if (intensity_green > 255) intensity_green = 255;
                        if (intensity_green < 0) intensity_green = 0;
                        
                    }
                    printf("Vermelho: %d, Verde: %d \n", intensity_red, intensity_green);
                } else {
                    //mantém intensidade joystick parado
                    if (is_red) {
                        intensity_red = intensity_red;
                    } else {
                        intensity_green = intensity_green;
                    }
                    
                }
                // Ajusta a cor do LED atual
                adjust_color();
            
            }
            // reinicia o temporizador do loop principal
            start_time = time_us_64();
           
        
            // alterna entre as cores vermelho e verde
            if (!gpio_get(BUTTON_A)) {
                uint64_t current_time = time_us_64();
                if(current_time - last_button_a_time >= debounce_interval){
                    is_red = !is_red;
                    printf(is_red ? "vermelho selecionado\n" : "verde selecionado\n");
                    last_button_a_time = current_time; // atualiza o tempo 
                }
            }
            
            // ajusta cor a led atual
            adjust_color(); 
        }
    }

    return 0;
}
