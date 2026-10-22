#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Estrutura para definir os dados de um evento de segurança
typedef struct Evento {
    int id;             // Identificador único do evento
    char ip[16];        // Endereço IP de origem no formato "XXX.XXX.XXX.XXX"
    int severidade;     // Nível de 1 a 5 (onde 1 é o mais crítico)
    char horario[9];    // Horário no formato "HH:MM:SS"
    int tratado;        // Status: 0 = pendente | 1 = tratado pela equipe
} Evento;

// Nó da lista encadeada (armazena os dados e o ponteiro de ligação)
typedef struct No {
    Evento evento;      // Dados do evento de segurança
    struct No *prox;    // Ponteiro para o próximo nó da lista (NULL no último)
} No;

// Estrutura principal que gerencia o início e o tamanho da lista
typedef struct Lista {
    No *inicio;         // Ponteiro para o primeiro nó da lista
    int qtd;            // Quantidade total de eventos na lista
} Lista;


// Cria e inicializa uma nova lista vazia na memória dinâmica (malloc)
Lista* criar_lista() {
    Lista *nova_lista = (Lista*) malloc(sizeof(Lista));
    if (nova_lista != NULL) { //garantir que caso o malloc n consiga alocar memória, n haja segmentation Fault - ct
        nova_lista->inicio = NULL; // A lista começa vazia
        nova_lista->qtd = 0;       // Quantidade inicial zerada
    }
    return nova_lista;
}

// Função de validação dos dados de um evento antes de inserir
int validar_evento(Lista *lista, Evento alerta) {
    // 1. Verifica se o ID já existe na lista (garante identificador único)
    if (lista != NULL) { //verifica lista vazia 
        No *atual = lista->inicio;// atribui atual para melhor compreensão
        while (atual != NULL) { // verifica se ja chegou ao ultimo elemento
            if (atual->evento.id == alerta.id) {
                return 0; // ID duplicado -> Inválido
            }
            atual = atual->prox;
        } 
    }

    // 2. Valida a severidade (deve estar entre 1 e 5)
    if (alerta.severidade < 1 || alerta.severidade > 5) {
        return 0;
    }

    // 3. Valida o formato do IP e os valores dos 4 octetos (0 a 255)
    int p1, p2, p3, p4;
    if (sscanf(alerta.ip, "%d.%d.%d.%d", &p1, &p2, &p3, &p4) != 4) {
        return 0; // Formato incorreto
    }
    if (p1 < 0 || p1 > 255 || p2 < 0 || p2 > 255 || 
        p3 < 0 || p3 > 255 || p4 < 0 || p4 > 255) {
        return 0; // Número fora do intervalo válido
    }

    // 4. Valida o formato e intervalo do horário (00:00:00 até 23:59:59)
    int h, m, s;
    if (sscanf(alerta.horario, "%d:%d:%d", &h, &m, &s) != 3) {
        return 0; // Formato incorreto
    }
    if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
        return 0; // Hora, minuto ou segundo inválidos
    }

    return 1; // Evento válido!
}


//  Insere um evento no início da lista (posição 0)
int inserir_inicio(Lista *lista, Evento alerta) {
    if (lista == NULL || !validar_evento(lista, alerta)) {
        return 0; // Lista não existe ou dados inválidos
    }

    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        return 0; // Falha na alocação de memória
    }

    novo->evento = alerta;
    novo->prox = lista->inicio; // Aponta para o antigo primeiro elemento
    lista->inicio = novo;       // Torna-se o novo primeiro elemento
    lista->qtd++;

    return 1; // Sucesso
}

//  Insere um evento no final da lista
int inserir_final(Lista *lista, Evento alerta) {
    if (lista == NULL || !validar_evento(lista, alerta)) {
        return 0;
    }

    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }

    novo->evento = alerta;
    novo->prox = NULL; // Como será o último nó, o próximo é NULL

    // Se a lista estiver vazia, o novo nó será o início
    if (lista->inicio == NULL) {
        lista->inicio = novo;
    } else {
        // Percorre até o último nó da lista
        No *atual = lista->inicio;
        while (atual->prox != NULL) {
            atual = atual->prox;
        }
        atual->prox = novo; // Faz o antigo último nó apontar para o novo
    }

    lista->qtd++;
    return 1;
}

// Insere um evento em uma posição p informada (posição 0 é o início)
int inserir_posicao(Lista *lista, Evento alerta, int p) {
    // Valida a lista, se a posição é válida (0 até qtd) e se o evento é válido
    if (lista == NULL || p < 0 || p > lista->qtd || !validar_evento(lista, alerta)) {
        return 0;
    }

    // Casos particulares: insere no início ou no fim
    if (p == 0) {
        return inserir_inicio(lista, alerta);
    }
    if (p == lista->qtd) {
        return inserir_final(lista, alerta);
    }

    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }
    novo->evento = alerta;

    // Caminha na lista até o nó anterior à posição p (posição p - 1)
    No *atual = lista->inicio;
    for (int i = 0; i < p - 1; i++) {
        atual = atual->prox;
    }

    // Insere o novo nó entre 'atual' e 'atual->prox'
    novo->prox = atual->prox;
    atual->prox = novo;
    lista->qtd++;

    return 1;
}

// Insere ordenado por severidade (1 é a mais crítica).
// Em caso de empate na severidade, o horário mais antigo ("HH:MM:SS") vem primeiro.
int inserir_ordenado(Lista *lista, Evento alerta) {
    if (lista == NULL || !validar_evento(lista, alerta)) {
        return 0;
    }

    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }
    novo->evento = alerta;

    // Caso 1: Inserção na cabeça (lista vazia ou o evento é mais crítico/antigo que o 1º nó)
    if (lista->inicio == NULL || 
        alerta.severidade < lista->inicio->evento.severidade ||
        (alerta.severidade == lista->inicio->evento.severidade && 
         strcmp(alerta.horario, lista->inicio->evento.horario) < 0)) {
        
        novo->prox = lista->inicio;
        lista->inicio = novo;
        lista->qtd++;
        return 1;
    }

    // Caso 2: Procura a posição correta de inserção no meio ou final da lista
    No *atual = lista->inicio;
    while (atual->prox != NULL) {
        int seg_sev = atual->prox->evento.severidade;
        char *seg_horario = atual->prox->evento.horario;

        // Se o novo evento for mais crítico ou tiver horário menor que o próximo, para aqui
        if (alerta.severidade < seg_sev || 
           (alerta.severidade == seg_sev && strcmp(alerta.horario, seg_horario) < 0)) {
            break;
        }
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
    lista->qtd++;

    return 1;
}
int remover_id(Lista *lista, int id, Evento *removido){
    if (lista == NULL || lista->inicio == NULL) {
        return 0;
    }
    No *anterior = NULL;
    No *atual = lista->inicio;
    
    while (atual != NULL && atual->evento.id != id) {
        anterior = atual;
        atual = atual->prox;
    }
    
    if (atual == NULL) return 0; // ID não existe
    
    *removido = atual->evento; // Guarda os dados do evento removido (passagem por referência)
    
    if (anterior == NULL) {
        lista->inicio = atual->prox; // Removendo o primeiro nó
    } else {
        anterior->prox = atual->prox; // Removendo do meio ou fim
    }
    
    free(atual);
    lista->qtd--;
    return 1;
}

No* busca_id(Lista *lista, int id){
    if (lista == NULL) {
        return NULL;
    }
    No *atual = lista->inicio;
    while (atual != NULL) {
        if (atual->evento.id == id) {
            return atual;
        }
    }
    return NULL;
}