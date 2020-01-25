#include <stdlib.h>
#include "tsdq.h"


TsdqHandle* tsdq_create(int capacity) {
  TsdqHandle* tsdq = (TsdqHandle*) malloc(sizeof(TsdqHandle));

  tsdq->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  tsdq->full = malloc(sizeof(sem_t));
  sem_init(tsdq->full, 0, capacity);
  tsdq->empty = malloc(sizeof(sem_t));
  sem_init(tsdq->empty, 0, 0);

  tsdq->tail = malloc(sizeof(TsdqNode));
  tsdq->head = malloc(sizeof(TsdqNode));
  tsdq->head->next = tsdq->tail;
  tsdq->tail->prev = tsdq->head;
  return tsdq;
}

void tsdq_enqueue(TsdqHandle* tsdq, void* data) {
  sem_wait(tsdq->full);
  pthread_mutex_lock(tsdq->mutex);

  TsdqNode* new_node  = malloc(sizeof(TsdqNode));
  new_node->value = data;
  TsdqNode* next = tsdq->head->next;
  tsdq->head->next = new_node;
  new_node->prev = tsdq->head;
  new_node->next = next;
  next->prev = new_node;

  pthread_mutex_unlock(tsdq->mutex);
  sem_post(tsdq->empty);

}

void* tsdq_dequeue(TsdqHandle* tsdq) {
  sem_wait(tsdq->empty);
  pthread_mutex_lock(tsdq->mutex);

  TsdqNode* ret = tsdq->tail->prev;
  TsdqNode* new_tail = tsdq->tail->prev->prev;
  tsdq->tail->prev = new_tail;
  new_tail->next = tsdq->tail;
  void* value = ret->value;
  free(ret);

  pthread_mutex_unlock(tsdq->mutex);
  sem_post(tsdq->full);
  return value;
}

void tsdq_empty_int(TsdqHandle* tsdq) {
  while (tsdq->head->next != tsdq->tail) {
    tsdq_dequeue(tsdq);
  }
}

void tsdq_destroy(TsdqHandle* tsdq) {
  tsdq_empty_int(tsdq);
  free(tsdq->mutex);
  free(tsdq->full);
  free(tsdq->empty);
  free(tsdq->head);
  free(tsdq->tail);
}
