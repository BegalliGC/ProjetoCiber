#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definição das estruturas:

//Estrutura pra definir o que tem dentro do evento
typedef struct Evento{
    int id;
    char ip[16]; //seguir o formato "192.168.0.10"
    int severidade; // 1 a 5
    char horario[9]; // formato "HH:MM:SS"
    int tratado; // 0 = pendente | 1 = tratado pela equipe
}Evento;

//define o no de ligação entre os nós
typedef struct No {
    Evento evento;
    struct No *prox;     
} No;


typedef struct Lista {
    No *inicio;
    int qtd;
}Lista;

//Definição das funções:

Lista* criar_lista(){
    Lista *nova_lista = malloc(sizeof(Lista));
    if (nova_lista != NULL){
        nova_lista->inicio = NULL;
        nova_lista->qtd = 0;
    }
    return nova_lista;
}

int validar_evento(Lista *lista, Evento alerta){
    if (lista != NULL) { 
       No * atual = lista->inicio;
       while (atual != NULL) {
        if (atual->evento.id == alerta.id) {
            return 0;
        }
        atual=atual->prox;
       } 
    }

    if (alerta.severidade < 1 || alerta.severidade > 5) {
        return 0;
    }

    int p1, p2, p3, p4;
    if (sscanf(alerta.ip, "%d.%d.%d.%d", &p1, &p2, &p3, &p4)!=4) {
        return 0;
    }
    if (p1 < 0 || p1 > 255 || p2 < 0 || p2 > 255 || p3 < 0 || p3 > 255 || p4 < 0 || p4 > 255) {
        return 0;
    }

    int h, m, s;
    if (sscanf(alerta.horario, "%d:%d:%d", &h, &m, &s)!=3) {
        return 0;
    }
    if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
        return 0;
    }
    return 1;
}