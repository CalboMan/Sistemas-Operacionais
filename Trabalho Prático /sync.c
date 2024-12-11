#include "sync.h"
#include <stdlib.h>
#include <stdio.h>

// Mutex
void inicMutex(Mutex *mutex) {
    mutex->handle = CreateMutex(NULL, FALSE, NULL);
    if (mutex->handle == NULL) {
        perror("Erro ao criar mutex");
        exit(EXIT_FAILURE);
    }
}

void trancMutex(Mutex *mutex) {
    WaitForSingleObject(mutex->handle, INFINITE);
}

void destrancMutex(Mutex *mutex) {
    ReleaseMutex(mutex->handle);
}

void destruirMutex(Mutex *mutex) {
    CloseHandle(mutex->handle);
}

// Condição
void inicCond(Condition *cond) {
    cond->semaf = CreateSemaphore(NULL, 0, INT_MAX, NULL);
    cond->controlPronto = CreateSemaphore(NULL, 0, INT_MAX, NULL);
    InitializeCriticalSection(&cond->lock);
    cond->countEsperar = 0;
    cond->countSinal = 0;
}

void esperCond(Condition *cond, Mutex *mutex) {
    EnterCriticalSection(&cond->lock);
    cond->countEsperar++;
    LeaveCriticalSection(&cond->lock);

    destrancMutex(mutex);
    WaitForSingleObject(cond->semaf, INFINITE);

    EnterCriticalSection(&cond->lock);
    cond->countSinal--;
    if (cond->countSinal == 0 && cond->countEsperar == 0) {
        ReleaseSemaphore(cond->controlPronto, 1, NULL);
    }
    LeaveCriticalSection(&cond->lock);

    trancMutex(mutex);
}

void sinalCond(Condition *cond) {
    EnterCriticalSection(&cond->lock);
    if (cond->countEsperar > cond->countSinal) {
        cond->countSinal++;
        ReleaseSemaphore(cond->semaf, 1, NULL);
    }
    LeaveCriticalSection(&cond->lock);
}

void broadCond(Condition *cond) {
    EnterCriticalSection(&cond->lock);
    if (cond->countEsperar > 0) {
        ReleaseSemaphore(cond->semaf, cond->countEsperar, NULL);
        cond->countSinal = cond->countEsperar;
        cond->countEsperar = 0;
    }
    LeaveCriticalSection(&cond->lock);

    WaitForSingleObject(cond->controlPronto, INFINITE);
}

void destruirCond(Condition *cond) {
    CloseHandle(cond->semaf);
    CloseHandle(cond->controlPronto);
    DeleteCriticalSection(&cond->lock);
}


// Semáforo
void inicSemaf(Semaphore *sem, int initial_count) {
    sem->handle = CreateSemaphore(NULL, initial_count, INT_MAX, NULL);
    if (sem->handle == NULL) {
        perror("Erro ao criar semáforo");
        exit(EXIT_FAILURE);
    }
}

void esperSemaf(Semaphore *sem) {
    WaitForSingleObject(sem->handle, INFINITE);
}

void sinalSemaf(Semaphore *sem) {
    ReleaseSemaphore(sem->handle, 1, NULL);
}

void destruirSemaf(Semaphore *sem) {
    CloseHandle(sem->handle);
}
