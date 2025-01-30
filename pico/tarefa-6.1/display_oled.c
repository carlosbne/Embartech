#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Definição dos pinos
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

const uint LED_R_PIN = 13;
const uint LED_G_PIN = 11;
const uint LED_B_PIN = 12;

const uint BTN_A_PIN = 5;

int A_state = 0;   // Variável de estado Botão A

//Area display
struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

void LimpaDisplay(void){
  memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

}

void MensagemDisplay(const char *text[], int lines){
    int y = 0;
    for (int i = 0; i < lines; i++){
        ssd1306_draw_string(ssd, 5, y, (char *)text[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);

}


void SinalAberto(void){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);  
    LimpaDisplay();
    const char *text[] ={
        " SINAL ABERTO -  ",
        " ATRAVESSAR ", 
        " COM CUIDADO "
        };
    MensagemDisplay(text, 3);
}

void SinalAtencao(void){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    LimpaDisplay();
    const char *text[] ={
        " SINAL DE  ",
        " ATENCAO - ", 
        " PREPARE-SE "
        };
    MensagemDisplay(text, 3);
}

void SinalFechado(void){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
    LimpaDisplay();
    const char *text[] ={
        " SINAL ",
        " FECHADO - ",
        " AGUARDE "
        };
    MensagemDisplay(text, 3);
}


int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}




int main(){
    
    // INICIANDO LEDS
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // INICIANDO BOTÄO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    

    stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização do i2c 
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
  
    ssd1306_init();

    //config renderização
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    /* uint8_t ssd[ssd1306_buffer_length]; */
    /* memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
 */
/* restart: */

// Parte do código para exibir a mensagem no display (opcional: mudar ssd1306_height para 32 em ssd1306_i2c.h)
// /**
    /* char *text[] = {
        " SINAL ABERTO -  ",
        " ATRAVESSAR COM ", 
        " CUIDADO  "};
 */
    /* int y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);
 */

    while(true){

        SinalFechado();
        A_state = WaitWithRead(8000);   //espera com leitura do botäo
        //sleep_ms(8000);


        if(A_state){               //ALGUEM APERTOU O BOTAO - SAI DO SEMAFORO NORMAL
            //SINAL AMARELO PARA OS PEDESTRES 2s
            SinalAtencao();
            sleep_ms(3000);

            //SINAL VERDE PARA OS PEDESTRES
            SinalAberto();
            sleep_ms(10000);

        }else{     //NINGUEM APERTOU O BOTAO - CONTINUA NO SEMAFORO NORMAL
                                      
            SinalAtencao();
            sleep_ms(2000);

            //SINAL VERMELHO PARA OS PEDESTRES POR 15s
            SinalAberto();
            sleep_ms(8000);
        }
         sleep_ms(1000);
    }

    return 0;

}
