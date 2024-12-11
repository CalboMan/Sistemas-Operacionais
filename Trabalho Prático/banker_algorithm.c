#include "sync.h"
#include "banker_algorithm.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Funções do Algoritmo do Banqueiro
void inicBanco(BankerAlgorithm* banker, int countProces, int countRecur, int* total_resources) {
    banker->countProces = countProces;
    banker->countRecur = countRecur;

    banker->dispon = malloc(countRecur * sizeof(int));
    banker->max = malloc(countProces * sizeof(int*));
    banker->aloc = malloc(countProces * sizeof(int*));
    banker->neces = malloc(countProces * sizeof(int*));

    for (int i = 0; i < countProces; i++) {
        banker->max[i] = malloc(countRecur * sizeof(int));
        banker->aloc[i] = malloc(countRecur * sizeof(int));
        banker->neces[i] = malloc(countRecur * sizeof(int));
    }

    for (int i = 0; i < countRecur; i++) {
        banker->dispon[i] = total_resources[i];
    }

    for (int i = 0; i < countProces; i++) {
        for (int j = 0; j < countRecur; j++) {
            banker->aloc[i][j] = 0;
            banker->neces[i][j] = 0;
            banker->max[i][j] = 0;
        }
    }
}

void setMaxDemanda(BankerAlgorithm* banker, int process_id, int* max_demand) {
    for (int i = 0; i < banker->countRecur; i++) {
        banker->max[process_id][i] = max_demand[i];
        banker->neces[process_id][i] = max_demand[i];
    }
}

bool estadoSeguro(BankerAlgorithm* banker) {
    int* work = malloc(banker->countRecur * sizeof(int));
    bool* finish = malloc(banker->countProces * sizeof(bool));

    for (int i = 0; i < banker->countRecur; i++) {
        work[i] = banker->dispon[i];
    }

    for (int i = 0; i < banker->countProces; i++) {
        finish[i] = false;
    }

    for (int i = 0; i < banker->countProces; i++) {
        for (int j = 0; j < banker->countProces; j++) {
            if (!finish[j]) {
                bool can_satisfy = true;
                for (int k = 0; k < banker->countRecur; k++) {
                    if (banker->neces[j][k] > work[k]) {
                        can_satisfy = false;
                        break;
                    }
                }

                if (can_satisfy) {
                    for (int k = 0; k < banker->countRecur; k++) {
                        work[k] += banker->aloc[j][k];
                    }
                    finish[j] = true;
                    i = -1; // Reinicia o loop
                }
            }
        }
    }

    bool safe = true;
    for (int i = 0; i < banker->countProces; i++) {
        if (!finish[i]) {
            safe = false;
            break;
        }
    }

    free(work);
    free(finish);

    return safe;
}

bool pedirRecursos(BankerAlgorithm* banker, int process_id, int* request) {
    for (int i = 0; i < banker->countRecur; i++) {
        if (request[i] > banker->neces[process_id][i] || request[i] > banker->dispon[i]) {
            return false;
        }
    }

    for (int i = 0; i < banker->countRecur; i++) {
        banker->dispon[i] -= request[i];
        banker->aloc[process_id][i] += request[i];
        banker->neces[process_id][i] -= request[i];
    }

    if (!estadoSeguro(banker)) {
        for (int i = 0; i < banker->countRecur; i++) {
            banker->dispon[i] += request[i];
            banker->aloc[process_id][i] -= request[i];
            banker->neces[process_id][i] += request[i];
        }
        return false;
    }

    return true;
}

void liberarRecursos(BankerAlgorithm* banker, int process_id, int* release) {
    for (int i = 0; i < banker->countRecur; i++) {
        banker->dispon[i] += release[i];
        banker->aloc[process_id][i] -= release[i];
        banker->neces[process_id][i] += release[i];
    }
}

void destruirBanco(BankerAlgorithm* banker) {
    free(banker->dispon);
    for (int i = 0; i < banker->countProces; i++) {
        free(banker->max[i]);
        free(banker->aloc[i]);
        free(banker->neces[i]);
    }
    free(banker->max);
    free(banker->aloc);
    free(banker->neces);
}
