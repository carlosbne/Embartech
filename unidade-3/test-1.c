#include <stdio.h>

int soma(int a, int b){
    return a + b;
}

int main(){
    int a;
    int b;

    printf("primeiro número:\n");
    scanf("%d", &a);
    printf("segundo número:\n");
    scanf("%d", &b);

    int s = soma(a, b);

    printf("%d\n", s);
    printf("Hello World\n");
}