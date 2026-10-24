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
    if (nova_lista != NULL) {
        nova_lista->inicio = NULL; // A lista começa vazia
        nova_lista->qtd = 0;       // Quantidade inicial zerada
    }
    return nova_lista;
}

// Função de validação dos dados de um evento antes de inserir
int validar_evento(Lista *lista, Evento alerta) {
    // 1. Verifica se o ID já existe na lista (garante identificador único)
    if (lista != NULL) { 
        No *atual = lista->inicio;
        while (atual != NULL) {
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



// Insere um evento no início da lista (posição 0)
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

// Insere um evento no final da lista
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

    if (lista->inicio == NULL) {
        lista->inicio = novo;
    } else {
        No *atual = lista->inicio;
        while (atual->prox != NULL) {
            atual = atual->prox;
        }
        atual->prox = novo;
    }

    lista->qtd++;
    return 1;
}

// Insere um evento em uma posição p informada (posição 0 é o início)
int inserir_posicao(Lista *lista, Evento alerta, int p) {
    if (lista == NULL || p < 0 || p > lista->qtd || !validar_evento(lista, alerta)) {
        return 0;
    }

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

    No *atual = lista->inicio;
    for (int i = 0; i < p - 1; i++) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
    lista->qtd++;

    return 1;
}

// Insere ordenado por severidade (1 é a mais crítica) com desempate por horário
int inserir_ordenado(Lista *lista, Evento alerta) {
    if (lista == NULL || !validar_evento(lista, alerta)) {
        return 0;
    }

    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }
    novo->evento = alerta;

    // Cabeça da lista
    if (lista->inicio == NULL || 
        alerta.severidade < lista->inicio->evento.severidade ||
        (alerta.severidade == lista->inicio->evento.severidade && 
         strcmp(alerta.horario, lista->inicio->evento.horario) < 0)) {
        
        novo->prox = lista->inicio;
        lista->inicio = novo;
        lista->qtd++;
        return 1;
    }

    // Meio ou fim
    No *atual = lista->inicio;
    while (atual->prox != NULL) {
        int seg_sev = atual->prox->evento.severidade;
        char *seg_horario = atual->prox->evento.horario;

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



// Remove um evento procurando pelo ID e retorna os dados em *removido
int remover_id(Lista *lista, int id, Evento *removido) {
    if (lista == NULL || lista->inicio == NULL) {
        return 0;
    }

    No *anterior = NULL;
    No *atual = lista->inicio;

    while (atual != NULL && atual->evento.id != id) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) return 0; // ID não encontrado

    if (removido != NULL) {
        *removido = atual->evento; // Guarda os dados do evento por referência
    }

    if (anterior == NULL) {
        lista->inicio = atual->prox; // Removendo o primeiro nó
    } else {
        anterior->prox = atual->prox; // Removendo do meio ou do fim
    }

    free(atual);
    lista->qtd--;
    return 1;
}

// Remove todos os eventos cujo status seja 'tratado == 1'
int remover_tratados(Lista *lista) {
    if (lista == NULL || lista->inicio == NULL) {
        return 0;
    }

    int total_removidos = 0;
    No *atual = lista->inicio;
    No *anterior = NULL;

    while (atual != NULL) {
        if (atual->evento.tratado == 1) {
            No *aux = atual;
            if (anterior == NULL) {
                lista->inicio = atual->prox;
                atual = lista->inicio;
            } else {
                anterior->prox = atual->prox;
                atual = atual->prox;
            }
            free(aux);
            lista->qtd--;
            total_removidos++;
        } else {
            anterior = atual;
            atual = atual->prox;
        }
    }

    return total_removidos; // Retorna a quantidade total de eventos removidos
}

// Encontra e remove o evento mais crítico (menor valor de severidade)
int remover_mais_critico(Lista *lista, Evento *removido) {
    if (lista == NULL || lista->inicio == NULL) {
        return 0;
    }

    No *atual = lista->inicio;
    No *no_critico = lista->inicio;
    No *ant_critico = NULL;

    // Procura o nó com menor valor de severidade (e desempata por horário mais antigo)
    while (atual->prox != NULL) {
        No *prox_no = atual->prox;
        if (prox_no->evento.severidade < no_critico->evento.severidade ||
           (prox_no->evento.severidade == no_critico->evento.severidade &&
            strcmp(prox_no->evento.horario, no_critico->evento.horario) < 0)) {
            no_critico = prox_no;
            ant_critico = atual;
        }
        atual = atual->prox;
    }

    // Copia os dados do evento removido por referência
    if (removido != NULL) {
        *removido = no_critico->evento;
    }

    // Desvincula o nó mais crítico da lista
    if (ant_critico == NULL) {
        lista->inicio = no_critico->prox;
    } else {
        ant_critico->prox = no_critico->prox;
    }

    free(no_critico);
    lista->qtd--;
    return 1;
}



// Busca um evento pelo ID e retorna o ponteiro para o Nó (NULL se não achar)
No* busca_id(Lista *lista, int id) {
    if (lista == NULL) {
        return NULL;
    }
    No *atual = lista->inicio;
    while (atual != NULL) {
        if (atual->evento.id == id) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

// Altera o status de um evento para 'tratado = 1'
int marcar_tratado(Lista *lista, int id) {
    No *no = busca_id(lista, id);
    if (no != NULL) {
        no->evento.tratado = 1;
        return 1; // Sucesso
    }
    return 0; // ID não encontrado
}

// Imprime todos os eventos originados de um IP e retorna a contagem total
int exibir_por_ip(Lista *lista, const char *ip) {
    if (lista == NULL || lista->inicio == NULL) {
        printf("\nLista vazia ou inexistente.\n");
        return 0;
    }

    int encontrados = 0;
    No *atual = lista->inicio;

    printf("\n--- Ocorrencias registradas para o IP: %s ---\n", ip);
    while (atual != NULL) {
        if (strcmp(atual->evento.ip, ip) == 0) {
            printf("ID: %-5d | Severidade: %d | Horario: %s | Status: %s\n",
                   atual->evento.id,
                   atual->evento.severidade,
                   atual->evento.horario,
                   atual->evento.tratado ? "TRATADO" : "PENDENTE");
            encontrados++;
        }
        atual = atual->prox;
    }

    if (encontrados == 0) {
        printf("Nenhum evento encontrado para o IP informado.\n");
    }
    printf("Total de ocorrencias: %d\n", encontrados);

    return encontrados;
}

// Imprime todos os eventos da lista em formato organizado
void exibir_lista(Lista *lista) {
    if (lista == NULL || lista->inicio == NULL) {
        printf("\n[AVISO] A lista de eventos esta vazia.\n");
        return;
    }

    printf("\n=================================================================\n");
    printf("                  MONITOR DE EVENTOS DE SEGURANCA                \n");
    printf("=================================================================\n");
    printf("Total de registros na lista: %d\n", lista->qtd);
    printf("-----------------------------------------------------------------\n");

    No *atual = lista->inicio;
    while (atual != NULL) {
        printf("ID: %-5d | IP: %-15s | Sev: %d | Hora: %s | Status: %s\n",
               atual->evento.id,
               atual->evento.ip,
               atual->evento.severidade,
               atual->evento.horario,
               atual->evento.tratado ? "TRATADO" : "PENDENTE");
        atual = atual->prox;
    }
    printf("=================================================================\n\n");
}

// Percorre a lista liberando a memória de cada nó e depois a própria lista
void liberar_lista(Lista *lista) {
    if (lista == NULL) return;

    No *atual = lista->inicio;
    while (atual != NULL) {
        No *aux = atual;
        atual = atual->prox;
        free(aux);
    }
    free(lista);
}


int main() {
    Lista *minha_lista = criar_lista();
    int opcao;

    do {
        printf("\n================ MENU MONITOR DE EVENTOS ================\n");
        printf("1. Inserir evento no inicio\n");
        printf("2. Inserir evento no final\n");
        printf("3. Inserir evento ordenado (por severidade)\n");
        printf("4. Inserir evento em posicao especifica\n");
        printf("5. Exibir todos os eventos\n");
        printf("6. Marcar evento como tratado (por ID)\n");
        printf("7. Buscar e exibir eventos por IP\n");
        printf("8. Remover evento por ID\n");
        printf("9. Remover todos os eventos tratados\n");
        printf("10. Remover evento mais critico\n");
        printf("0. Sair\n");
        printf("=========================================================\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("\n[ERRO] Entrada invalida!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1: {
                Evento ev;
                printf("\n--- Inserir no Inicio ---\n");
                printf("ID (inteiro): "); scanf("%d", &ev.id);
                printf("IP (ex: 192.168.1.10): "); scanf("%s", ev.ip);
                printf("Severidade (1 a 5): "); scanf("%d", &ev.severidade);
                printf("Horario (HH:MM:SS): "); scanf("%s", ev.horario);
                ev.tratado = 0;

                if (inserir_inicio(minha_lista, ev)) {
                    printf("\n[SUCESSO] Evento inserido no inicio!\n");
                } else {
                    printf("\n[ERRO] Dados invalidos ou ID ja cadastrado.\n");
                }
                break;
            }
            case 2: {
                Evento ev;
                printf("\n--- Inserir no Final ---\n");
                printf("ID (inteiro): "); scanf("%d", &ev.id);
                printf("IP (ex: 192.168.1.10): "); scanf("%s", ev.ip);
                printf("Severidade (1 a 5): "); scanf("%d", &ev.severidade);
                printf("Horario (HH:MM:SS): "); scanf("%s", ev.horario);
                ev.tratado = 0;

                if (inserir_final(minha_lista, ev)) {
                    printf("\n[SUCESSO] Evento inserido no final!\n");
                } else {
                    printf("\n[ERRO] Dados invalidos ou ID ja cadastrado.\n");
                }
                break;
            }
            case 3: {
                Evento ev;
                printf("\n--- Inserir Ordenado por Severidade ---\n");
                printf("ID (inteiro): "); scanf("%d", &ev.id);
                printf("IP (ex: 192.168.1.10): "); scanf("%s", ev.ip);
                printf("Severidade (1 a 5): "); scanf("%d", &ev.severidade);
                printf("Horario (HH:MM:SS): "); scanf("%s", ev.horario);
                ev.tratado = 0;

                if (inserir_ordenado(minha_lista, ev)) {
                    printf("\n[SUCESSO] Evento inserido ordenadamente!\n");
                } else {
                    printf("\n[ERRO] Dados invalidos ou ID ja cadastrado.\n");
                }
                break;
            }
            case 4: {
                Evento ev;
                int pos;
                printf("\n--- Inserir em Posicao Especifica ---\n");
                printf("Posicao desejada (0 para inicio): "); scanf("%d", &pos);
                printf("ID (inteiro): "); scanf("%d", &ev.id);
                printf("IP (ex: 192.168.1.10): "); scanf("%s", ev.ip);
                printf("Severidade (1 a 5): "); scanf("%d", &ev.severidade);
                printf("Horario (HH:MM:SS): "); scanf("%s", ev.horario);
                ev.tratado = 0;

                if (inserir_posicao(minha_lista, ev, pos)) {
                    printf("\n[SUCESSO] Evento inserido na posicao %d!\n", pos);
                } else {
                    printf("\n[ERRO] Posicao invalida, dados invalidos ou ID ja cadastrado.\n");
                }
                break;
            }
            case 5:
                exibir_lista(minha_lista);
                break;
            case 6: {
                int id;
                printf("\nInforme o ID do evento para marcar como tratado: ");
                scanf("%d", &id);
                if (marcar_tratado(minha_lista, id)) {
                    printf("\n[SUCESSO] Evento %d marcado como TRATADO.\n", id);
                } else {
                    printf("\n[ERRO] ID nao encontrado.\n");
                }
                break;
            }
            case 7: {
                char ip_busca[16];
                printf("\nInforme o IP que deseja buscar: ");
                scanf("%s", ip_busca);
                exibir_por_ip(minha_lista, ip_busca);
                break;
            }
            case 8: {
                int id;
                Evento removido;
                printf("\nInforme o ID do evento a ser removido: ");
                scanf("%d", &id);
                if (remover_id(minha_lista, id, &removido)) {
                    printf("\n[SUCESSO] Evento ID %d (%s) removido com sucesso!\n", removido.id, removido.ip);
                } else {
                    printf("\n[ERRO] ID nao encontrado na lista.\n");
                }
                break;
            }
            case 9: {
                int qtd_remv = remover_tratados(minha_lista);
                printf("\n[SUCESSO] Foram removidos %d eventos que ja estavam tratados.\n", qtd_remv);
                break;
            }
            case 10: {
                Evento removido;
                if (remover_mais_critico(minha_lista, &removido)) {
                    printf("\n[SUCESSO] Evento mais critico removido!\n");
                    printf("ID: %d | Severidade: %d | Horario: %s\n", removido.id, removido.severidade, removido.horario);
                } else {
                    printf("\n[ERRO] A lista esta vazia.\n");
                }
                break;
            }
            case 0:
                printf("\nEncerrando o programa e liberando memoria...\n");
                liberar_lista(minha_lista);
                printf("Ate mais!\n");
                break;
            default:
                printf("\n[ERRO] Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}