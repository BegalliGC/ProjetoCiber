# Monitor de Eventos de Segurança 🛡️

Mini-projeto de Estrutura de Dados e Análise de Algoritmo. Este sistema, desenvolvido em C, atua como um monitor de eventos de segurança, simulando os registos gerados por um firewall ou IDS (Intrusion Detection System).

A arquitetura do projeto assenta numa lista encadeada simples com alocação dinâmica para gerir os alertas em tempo real. O código foi escrito do zero, sem recurso a bibliotecas de listas previamente implementadas, focando-se no controlo manual de ponteiros e gestão eficiente de memória.

## 🚀 Funcionalidades

O sistema é operado através de um menu interativo que garante as seguintes operações em memória:

### 📥 Inserção e Validação de Alertas
- **Filtro Rigoroso:** Rejeita inserções inválidas através da validação estrita dos dados de entrada (ID único, formato de IP, limites de severidade e formatação de horário).
- **Fila de Prioridade (Ordenada):** Insere eventos priorizando o nível de severidade mais crítico; em caso de empate, o evento com o horário mais antigo é colocado em primeiro lugar.
- **Inserção Livre:** Suporta a alocação de nós no início, no final ou em qualquer posição arbitrária da lista.

### 🗑️ Mitigação e Remoção
- **Limpeza Específica:** Permite extrair e remover um evento da lista procurando pelo seu ID.
- **Limpeza em Massa:** Remove de uma só vez todos os eventos cujo estado já foi alterado para "tratado", libertando a memória.
- **Resposta a Incidentes:** Identifica, extrai e remove o evento mais crítico da lista instantaneamente.

### 🔍 Auditoria e Manutenção
- **Deteção de Força Bruta:** Filtra e exibe todos os eventos originados de um IP específico, contabilizando o total de ocorrências desse endereço para análise de tráfego anómalo.
- **Tratamento de Eventos:** Procura um evento pendente pelo seu ID e marca-o como tratado pela equipa.
- **Prevenção de Memory Leaks:** Ao encerrar o ciclo do menu, o sistema percorre todos os nós remanescentes e liberta toda a memória dinâmica alocada antes da finalização do processo.

## 🛠️ Estruturas de Dados Utilizadas
A base do monitor é construída sobre três estruturas fundamentais:
- `struct Evento`: Armazena os metadados do alerta (ID, IP de origem, severidade, horário e estado de tratamento).
- `struct No`: O elo da lista, contendo os dados do evento e o ponteiro dinâmico para o endereço do alerta seguinte.
- `struct Lista`: O controlador principal, que guarda o ponteiro para o início da lista e a quantidade total de eventos registados.
