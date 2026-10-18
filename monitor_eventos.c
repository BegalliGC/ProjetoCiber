#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Estrutura pra definir o que tem dentro do evento
typedef struct Evento{
    int id;
    char ip[16]; //seguir o formato "192.168.0.10"
    int severidade; // 1 a 5
    char horario; // formato "HH:MM:SS"
    int tratado; // 0 = pendente | 1 = tratado pela equipe
}Evento;


//define o no de ligação entre os nós
typedef struct No {
    Evento evento;
    struct No *prox;     
} No;

