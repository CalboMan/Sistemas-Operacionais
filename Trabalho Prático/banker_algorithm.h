#ifndef BANKER_H
#define BANKER_H

#include <stdbool.h>

// Estrutura do Algoritmo do Banqueiro
typedef struct {
    int recursosTotais; // Total de recursos disponíveis
    int* dispon;      // Recursos disponíveis para alocação
    int** max;           // Máximo que cada processo pode requisitar
    int** aloc;    // Recursos atualmente alocados
    int** neces;          // Recursos necessários (max - allocation)
    int countProces;   // Número de processos
    int countRecur;  // Número de tipos de recurso
} BankerAlgorithm;

// Funções do Algoritmo do Banqueiro
void inicBanco(BankerAlgorithm* banker, int process_count, int resource_count, int* total_resources);
void setMaxDemanda(BankerAlgorithm* banker, int process_id, int* max_demand);
bool estadoSeguro(BankerAlgorithm* banker);
bool pedirRecursos(BankerAlgorithm* banker, int process_id, int* request);
void liberarRecursos(BankerAlgorithm* banker, int process_id, int* release);
void destruirBanco(BankerAlgorithm* banker);

#endif // BANKER_H
