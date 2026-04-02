#include "wifi.h"

//---------Wifi------------
//Código baseado em https://github.com/BitDogLab/BitDogLab-C/tree/main/wifi_led
// Geração da resposta HTTP com o resultado
/* static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    char response[256];
    int result = calculate_colorblindness(reference_red, reference_green, intensity_red, intensity_green);
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
             "<!DOCTYPE html><html><body><h1>Resultado do Teste</h1>"
             "<p>Nivel de Daltonismo: %d</p></body></html>\r\n", result);
    tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);
    return ERR_OK;
}

err_t http_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // Configuração do novo cliente TCP
    printf("Novo cliente conectado!\n");

    // Associa o callback correto para lidar com os dados recebidos
    tcp_recv(newpcb, http_callback);

    return ERR_OK;
}

// Configuração do servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) return;
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) return;
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_accept_callback);
}
 */