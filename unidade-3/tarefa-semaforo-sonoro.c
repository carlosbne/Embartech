#include <stdio.h>
#include "pico/stdlib.h" 
#include "pico/cyw43_arch.h"
#include "hardware/adc.h" 
#include "hardware/pwm.h"


// Configuração dos Pinos
// pinos das LEDs semaforo
#define RED_PIN 8     // gp8
#define YELLOW_PIN 7  // gp7
#define GREEN_PIN 6   // gp6

// pino da LED pedestre
#define GREEN 5      // gp5 

// pino botão
#define BUTTON 27    // gp27

// pino do buzzar e frequência (em Hz)
#define BUZZER_PIN 21   //gp21
#define BUZZER_FREQUENCY 2000

//funções
void fluxoCarros();
void fluxoPedestre();



int main() {
  stdio_init_all();
  
  // Inicializa pinos
  gpio_init(RED_PIN);
  gpio_init(YELLOW_PIN);
  gpio_init(GREEN_PIN);

  gpio_init(GREEN);         //pedestre

  gpio_init(BUTTON);        //botão
  gpio_init(BUZZER_PIN);     //buzzer
 
  // Configura pinos
  gpio_set_dir(RED_PIN, GPIO_OUT);
  gpio_set_dir(YELLOW_PIN, GPIO_OUT);
  gpio_set_dir(GREEN_PIN, GPIO_OUT);

  gpio_set_dir(GREEN, GPIO_OUT);      //pedestre

  gpio_set_dir(BUTTON, GPIO_IN);      //botão
  gpio_set_dir(BUZZER_PIN, GPIO_OUT); //buzzer

  // Config inicial botão
  gpio_pull_up(BUTTON);


  while (true) {
    if (gpio_get(BUTTON) == 0) { // Verifica se o botão foi pressionado
      fluxoPedestre();  // Inicia o fluxo pedestre
    } else if (gpio_get(BUTTON) == 1) {  // Se o botão não estiver pressionado
      fluxoCarros();  // Inicia o fluxo de carros
    }
  }

  

    
  return 0;
}

void fluxoCarros(){
  //ativa a LED vermelha enquanto efetua a verificação do botão a cada 80ms
 for(int i = 0; i < 100; i++){
  gpio_put(RED_PIN, 0);
  gpio_put(YELLOW_PIN, 0);
  gpio_put(GREEN_PIN, 1);
    // Verifica o botão durante a execução do buzzer
    if (gpio_get(BUTTON) == 0) {
      fluxoPedestre(); // Interrompe o loop se o botão for pressionado
    }
    sleep_ms(80);
  }

  //ativa a LED vermelha enquanto efetua a verificação do botão a cada 40ms
  for(int i = 0; i < 50; i++){
    gpio_put(RED_PIN, 0);
    gpio_put(YELLOW_PIN, 1);
    gpio_put(GREEN_PIN, 0);
      // Verifica o botão durante a execução do buzzer
      if (gpio_get(BUTTON) == 0) {
        fluxoPedestre(); // Interrompe o loop se o botão for pressionado
      }
      sleep_ms(40);
  }
  
  //ativa a LED vermelha enquanto efetua a verificação do botão a cada 100ms
  for(int i = 0; i < 100; i++){
    gpio_put(RED_PIN, 1);
    gpio_put(YELLOW_PIN, 0);
    gpio_put(GREEN_PIN, 0);
   
      // Verifica o botão durante a execução do buzzer
      if (gpio_get(BUTTON) == 0) {
        fluxoPedestre(); // Interrompe o loop se o botão for pressionado
      }
    sleep_ms(100);
  }

 
  
}


//Fluxo pedestre inicia ao botão ser ativado
void fluxoPedestre(){
  //Acende apenas LED amarelo por 5s
  gpio_put(RED_PIN, 0);
  gpio_put(YELLOW_PIN, 1);
  gpio_put(GREEN_PIN, 0);
  sleep_ms(5000);
  

  //Acende apenas LED Vermelho e verde por 15s
  gpio_put(RED_PIN, 1);
  gpio_put(YELLOW_PIN, 0);
  gpio_put(GREEN, 1);

  for(int i = 0; i < 20; i++){
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(375);
    gpio_put(BUZZER_PIN, 0);
    sleep_ms(375);

    // Verifica o botão durante a execução do buzzer
    if (gpio_get(BUTTON) == 0) {
      break; // Interrompe o loop se o botão for pressionado
    }
  }
  

  //desativa LED verde pedestre 
  gpio_put(GREEN, 0);
  gpio_put(BUZZER_PIN, 0);

}