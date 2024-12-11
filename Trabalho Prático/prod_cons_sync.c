#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0;  // Índice de inserção
int out = 0; // Índice de remoção

HANDLE emptySlots;
HANDLE filledSlots;
HANDLE printMutex;

// Função do produtor com sincronização
void* producer_sync(void* param) {
    for (int i = 1; i < 100; i++) {
        WaitForSingleObject(emptySlots, INFINITE);  // Espera por um espaço vazio
        buffer[in] = i;

        WaitForSingleObject(printMutex, INFINITE);
        printf("Produziu (sync): %d\n", i);
        ReleaseMutex(printMutex);

        in = (in + 1) % BUFFER_SIZE;
        ReleaseSemaphore(filledSlots, 1, NULL);  // Sinaliza que há um item disponível
    }
    return NULL;
}

// Função do consumidor com sincronização
void* consumer_sync(void* param) {
    for (int i = 1; i < 100;) {
        int consumeCount = 1 + rand() % 3;  // Decide consumir entre 1 e 3 itens
        for (int j = 0; j < consumeCount && i < 100; j++) {
            if (WaitForSingleObject(filledSlots, 0) == WAIT_OBJECT_0) {  // Consome se há itens disponíveis
                int item = buffer[out];

                WaitForSingleObject(printMutex, INFINITE);
                printf("Consumiu (sync): %d\n", item);
                ReleaseMutex(printMutex);

                out = (out + 1) % BUFFER_SIZE;
                ReleaseSemaphore(emptySlots, 1, NULL);  // Sinaliza que há um espaço vazio
                i++;
            } else {
                break;
            }
        }
        Sleep(10);
    }
    return NULL;
}

// Teste com sincronização
void medirComSync() {
    emptySlots = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    filledSlots = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
    printMutex = CreateMutex(NULL, FALSE, NULL);

    srand((unsigned int)time(NULL));

    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    HANDLE producerThread, consumerThread;
    producerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)producer_sync, NULL, 0, NULL);
    consumerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)consumer_sync, NULL, 0, NULL);

    WaitForSingleObject(producerThread, INFINITE);
    WaitForSingleObject(consumerThread, INFINITE);

    QueryPerformanceCounter(&end);
    double time_taken = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    CloseHandle(producerThread);
    CloseHandle(consumerThread);
    CloseHandle(emptySlots);
    CloseHandle(filledSlots);
    CloseHandle(printMutex);

    printf("Tempo de execução com sincronização: %.6f segundos\n", time_taken);
}

// Teste sem sincronização
void medirSemSync() {
    srand((unsigned int)time(NULL));

    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    for (int i = 1, items_consumed = 0; i < 100 || items_consumed < 100;) {
        // Produção
        if (i < 1000) {
            buffer[in] = i;
            printf("Produziu (sem sync): %d\n", i);
            in = (in + 1) % BUFFER_SIZE;
            i++;
        }

        // Consumo espontâneo
        int consumeCount = 1 + rand() % 3;
        for (int j = 1; j < consumeCount && items_consumed < 100; j++) {
            if (in != out) {
                int item = buffer[out];
                printf("Consumiu (sem sync): %d\n", item);
                out = (out + 1) % BUFFER_SIZE;
                items_consumed++;
            } else {
                break;
            }
        }
        Sleep(10);
    }

    QueryPerformanceCounter(&end);
    double time_taken = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

    printf("Tempo de execução sem sincronização: %.6f segundos\n", time_taken);
}

int main() {
    printf("Iniciando teste com sincronização...\n");
    medirComSync();

    printf("\nIniciando teste sem sincronização...\n");
    medirSemSync();

    return 0;
}
