#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

//pinos e módulos controlador i2c selecionado
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15
#define BUZZER_PIN 21  // Define o pino do buzzer
#define SW 22          //botão do Joystick
#define VRX 26         // Pino ADC do eixo X do joystick
#define VRY 27         // Pino ADC do eixo Y do joystick

const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y
  

//definição dos LEDs RGB
const uint BLUE_LED_PIN= 12;   // LED azul no GPIO 12
const uint RED_LED_PIN  = 13; // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11;  // LED verde no GPIO 11




//variável para armazenar a posição do seletor do display
uint pos_y=12;

ssd1306_t disp; 
volatile bool J_EXIT = false;  // variável para indicar saída da função


//config pwm
const float DIVIDER_PWM = 16.0; // divisor de clock para o PWM
const uint16_t PERIOD = 4096;   // período do PWM (valor máximo)
uint16_t led_b_level, led_r_level = 100; // brilho inicial dos LEDs
uint slice_led_b, slice_led_r;  // identificadores dos slices PWM

const uint16_t PERIOD_LED = 2000;   // Período do PWM (valor máximo do contador para ajuste LED)
const uint16_t LED_STEP = 100;  // Passo de incremento/decremento do duty cycle do LED
uint16_t led_level = 100;       // Nível inicial do PWM (duty cycle)

//função para inicialização de todos os recursos do sistema
void inicializa(){
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRX);
    adc_gpio_init(VRY);
    i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

    //Inicialização do buzzer
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM); 

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
    gpio_init(SW);             // inicializa o pino do botão
    gpio_set_dir(SW, GPIO_IN); // config o pino do botão como entrada
    gpio_pull_up(SW);
    
}

//---------------- interrupção ------------------------------
// quando o botão for pressionado, ele sai da função atual
void button_callback(uint gpio, uint32_t events) {
    if (gpio == SW) {
        J_EXIT = true;  
    }
}

// Configuração do botão com interrupção
void setup_button() {
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &button_callback);
}




// -----------------Função Opção 3 LEDS ---------------------------

void setup_pwm_LED()
{
    uint slice;
    gpio_set_function(BLUE_LED_PIN, GPIO_FUNC_PWM); // Config o pino do BLUE_LED_PIN para função PWM
    slice = pwm_gpio_to_slice_num(BLUE_LED_PIN);    // Obtém o slice do PWM associado ao pino do BLUE_LED_PIN
    pwm_set_clkdiv(slice, DIVIDER_PWM);    // Define o divisor de clock do PWM
    pwm_set_wrap(slice, PERIOD);           // Config o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(BLUE_LED_PIN, led_level);    // Define o nível inicial do PWM para o pino do BLUE_LED_PIN
    pwm_set_enabled(slice, true);          // Habilita o PWM no slice correspondente
}


// função que faz o LED aumentar e diminuir o brilho
void fade_led()
{
    setup_pwm_LED();
    uint up_down = 1; // controle de brilho led
    J_EXIT = false;  // reset

    while (!J_EXIT) {// continua até que o botão seja pressionado
    if (J_EXIT) break;
        pwm_set_gpio_level(BLUE_LED_PIN, led_level); 
        sleep_ms(500);                      // Atraso de 500ms

        if (up_down)
        {
            led_level += LED_STEP; // incrementa o brilho do LED
            if (led_level >= PERIOD - LED_STEP)
                up_down = 0; // muda direção quando atingir o máximo
        }
        else
        {
            led_level -= LED_STEP; // decrementa o brilho do LED
            if (led_level <= LED_STEP)
                up_down = 1; // muda direção quando atingir o mínimo
        }
    }

    // quando o botão for pressionado, apaga os LEDs e volta ao menu
    pwm_set_gpio_level(BLUE_LED_PIN, 0);
}


//----------------Fuções Display Menu----------------------
//função escrita no display.
void print_texto(char *msg, uint pos_x, uint pos_y, uint scale){
    ssd1306_draw_string(&disp, pos_x, pos_y, scale, msg);//desenha texto
    ssd1306_show(&disp);//apresenta no Oled
}

//o desenho do retangulo fará o papel de seletor
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp,x1,y1,x2,y2);
    ssd1306_show(&disp);
}

//-------------------Funções Joystic LED ----------------

void set_leds(bool red, bool green, bool blue){
    gpio_put(RED_LED_PIN, red);
    gpio_put(GREEN_LED_PIN, green);
    gpio_put(BLUE_LED_PIN, blue);
}

// Função para controlar os LEDs com base no joystick
void led_control() {
    J_EXIT = false;  // Reseta a variável de saída antes de iniciar
    uint adc_x_raw, adc_y_raw;

    while (!J_EXIT) {  // loop até ser pressionado
        if (J_EXIT) break;  // sai com a interrupçlao

        adc_select_input(0);
        adc_x_raw = adc_read();
        adc_select_input(1);
        adc_y_raw = adc_read();

        if (adc_x_raw > 4000) {
            gpio_put(RED_LED_PIN, 1);
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 0);
        } 
        else if (adc_x_raw < 100) {
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 1);
        }
        else if (adc_y_raw > 4000) {
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 0);
        } 
        else if (adc_y_raw < 100) {
            gpio_put(RED_LED_PIN, 1);
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 0);
        }
        else {
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 0);
        }

        sleep_ms(100);  // Pequeno atraso para suavizar a leitura do joystick
    }

    // Quando o botão for pressionado, apaga os LEDs e volta ao menu
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);
}





//------------------ Ler Joystick -----------------------
// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
  // Leitura do valor do eixo X do joystick
  adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

  // Leitura do valor do eixo Y do joystick
  adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}


//-------------------Funções Buzzer---------------------
// Função para tocar uma nota
void play_tone(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        sleep_ms(duration);
        return;
    }

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 125000000 / frequency - 1);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), (125000000 / frequency) / 2);
    pwm_set_enabled(slice_num, true);

    sleep_ms(duration);
    pwm_set_enabled(slice_num, false);
}

// Função para tocar a melodia
void play_melody() {
    J_EXIT = false;  // reset var interrup
    const uint16_t melody[] = {
        740, 8, 740, 8, 587, 8, 494, 8, 0, 8, 494, 8, 0, 8, 659, 8,
        0, 8, 659, 8, 0, 8, 659, 8, 831, 8, 831, 8, 880, 8, 988, 8,
        880, 8, 880, 8, 880, 8, 659, 8, 0, 8, 587, 8, 0, 8, 740, 8,
        0, 8, 740, 8, 0, 8, 740, 8, 659, 8, 659, 8, 740, 8, 659, 8,
    };

    int tempo = 140;
    int wholenote = (60000 * 4) / tempo;
    int notes = sizeof(melody) / sizeof(melody[0]) / 2;

    for (int i = 0; i < notes * 2; i += 2) {
        if(J_EXIT){return;}//sai se pressionado

        int divider = melody[i + 1];
        int note_duration = (divider > 0) ? (wholenote / divider) : (wholenote / -divider) * 1.5;

        play_tone(melody[i], note_duration * 0.9);

        if(J_EXIT){return;}// sai se pressionado
        sleep_ms(note_duration * 0.1);
    }
}



//------------------------ Main -------------------------
int main()
{
    
    inicializa();
     char *text = ""; //texto do menu
     uint countdown = 0; //verificar seleções para baixo do joystick
     uint countup = 2; //verificar seleções para cima do joystick
 
    setup_button();
    ssd1306_clear(&disp);//Limpa a tela
    uint pos_y_antigo = pos_y; // guarda a posição do seletor
   
    while(true){
        //trecho de código aproveitado de https://github.com/BitDogLab/BitDogLab-C/blob/main/joystick/joystick.c
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max; //bar_y_pos determinará se o Joystick foi pressionado para cima ou para baixo

        //printf("Valor de y e: %d", bar_y_pos);
        //o valor de 20 é o estado de repouso do Joystick
        if(bar_y_pos < 20 && countdown <2){
            pos_y+=12;
            countdown+=1;
            countup-=1;
        }else
            if(bar_y_pos > 20 && countup <2){
                pos_y-=12;
                countup+=1;
                countdown-=1;
            }

        if(pos_y != pos_y_antigo || gpio_get(SW) == 0){ //Verifica se a posição mudou ou se botão foi pressionado
             //texto do Menu
            ssd1306_clear(&disp);//Limpa a tela
            print_texto(text="Menu", 52, 2, 1);
            print_retangulo(2, pos_y, 120, 18);
            print_texto(text="Joystick LED", 6, 18, 1.5);
            print_texto(text="Buzzer", 6, 30, 1.5);
            print_texto(text="LED RGB", 6, 42, 1.5);
            
            pos_y_antigo = pos_y; //atualiza a posição antiga
        }
      
       sleep_ms(250);

    //verifica se botão foi pressionado. Se sim, entra no switch case para verificar posição do seletor e chama acionamento dos leds.
    if (gpio_get(SW) == 0) {
        switch (pos_y) {
            case 12:
                J_EXIT = false;  // Garante que a variável está resetada antes de chamar a função
                led_control();
                break;
            case 24:
                play_melody();
                break;
            case 36:
                fade_led();
                break;
            default:
                break;
        }
    
        // Após executar qualquer função, volta ao menu principal
        ssd1306_clear(&disp);
        print_texto("Menu", 52, 2, 1);
        print_retangulo(2, pos_y, 120, 18);
        print_texto("Joystick LED", 6, 18, 1.5);
        print_texto("Buzzer", 6, 30, 1.5);
        print_texto("LED RGB", 6, 42, 1.5);
    }
    }
    return 0;
}
