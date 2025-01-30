#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int media(int arr[], int length);
int lerSensor();
int medicoes[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main(){
    printf("Olá embarcatech! vamos executar as 10 medições: \n");
    srand(time(0));

    for(int i = 0; i < 10; i++){
        medicoes[i] = lerSensor();
    }
    for(int i = 0; i < 10; i++){
        printf("%d, ", medicoes[i]);
    }
    printf(" -Média: %d\n", media(medicoes, 10));
    return 0;
}

int media(int arr[], int length){
    int acc = 0;
    for(int i = 0; i < length; i++){
        acc += arr[i];
    }
    return acc / length;
}

int lerSensor(){
    int num = rand() % ((35 - 25) + 1) + 25;
    return num;
}