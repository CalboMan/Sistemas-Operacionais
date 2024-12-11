#ifndef SYNC_H
#define SYNC_H

#include <windows.h>


// Mutex
typedef struct {
    HANDLE handle;
} Mutex;

void inicMutex(Mutex *mutex);
void trancMutex(Mutex *mutex);
void destrancMutex(Mutex *mutex);
void destruirMutex(Mutex *mutex);


// Condição
typedef struct {
    HANDLE semaf;
    HANDLE controlPronto;
    int countEsperar;
    int countSinal;
    CRITICAL_SECTION lock;
} Condition;

void inicCond(Condition *cond);
void esperCond(Condition *cond, Mutex *mutex);
void sinalCond(Condition *cond);
void broadCond(Condition *cond);
void destruirCond(Condition *cond);


// Semáforo
typedef struct {
    HANDLE handle;
} Semaphore;

void inicSemaf(Semaphore *sem, int initial_count);
void esperSemaf(Semaphore *sem);
void sinalSemaf(Semaphore *sem);
void destruirSemaf(Semaphore *sem);

#endif
