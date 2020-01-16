#ifndef __TSDQ_HPP__
#define __TSDQ_HPP__

#include <semaphore.h>
#include <pthread.h>

struct typedef {
  TsdqNode* head;
  TsdqNode* tail;
  sem_t* full;
  sem_t* empty;
  pthread_mutex_t* mutex;
} TsdqHandle;

struct typedef {
  void* value;
  TsdqNode* next;
  TsdqNode* prev;
} TsdqNode;

TsdqHandle* tsdq_create(int capacity);
void tsdq_destroy(TsdqHandle* tsdq);
void* tsdq_dequeue(TsdqHandle* tsdq);
void tsdq_enqueue(TsdqHandle* tsdq, void* data);


#endif
