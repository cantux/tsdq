#include "tsdq.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
static int g_counter = 0;

typedef struct {
  TsdqHandle* tsdq;
  int count;
  int id;
} producer_args;

typedef struct { int id, value; } idvp;

void* produce(void* args) {
  producer_args* a = (producer_args*)args;
  int i = 0;
  for (i = 0; i < a->count; i++) {
    idvp* x = malloc(sizeof(idvp));
    x->id = a->id;
    x->value = i;
    tsdq_enqueue(a->tsdq, x);
  }
}

typedef struct {
  TsdqHandle* tsdq;
  int count;
  int id;
} consumer_args;

void* consume(void* args) {
  consumer_args* a = (consumer_args*) args;
  int i = 0;

  for(i = 0; i < a->count; i++) {
    idvp* i_v = (idvp*)tsdq_dequeue(a->tsdq);
    printf("consumer id: %d, ith iter: %d, from producer: %d, got value: %d\n", a->id, i, i_v->id, i_v->value);
  } 
}

int main(void) {
  int size = 10;
  TsdqHandle* t_h = tsdq_create(size);
  int i = 0;
  for (i = 0; i < size; i++) {
    tsdq_enqueue(t_h, (void*)i);
  }
  for (i = 0; i < size; i++) {
    int res = (int)tsdq_dequeue(t_h);
    printf("res is: %d\n", res);
  }
  tsdq_destroy(t_h);


  // create a queue of size 30
  // create 5 producers that will produce 20 items
  // create 10 consumers that will consume 10 each
  const int queue_size = 30;
  TsdqHandle* tsdq_h = tsdq_create(queue_size);
  tsdq_h = tsdq_create(queue_size);

  const int produce_item = 20;
  const int producer_count = 5;
  pthread_t producer_pthread_ids[producer_count];
  producer_args* pa[producer_count];
  for(i = 0; i < producer_count; i++) {
    pa[i] = malloc(sizeof(producer_args));
    pa[i]->tsdq = tsdq_h;
    pa[i]->id = i;
    pa[i]->count = produce_item;
    pthread_create(&producer_pthread_ids[i], NULL, produce, pa[i]);
  }

  const int consume_item = 10;
  const int consumer_count = 10;
  pthread_t consumer_pthread_ids[consumer_count];
  consumer_args* ca[consumer_count];
  for(i = 0; i < consumer_count; i++) {
    ca[i] = malloc(sizeof(consumer_args));
    ca[i]->tsdq = tsdq_h;
    ca[i]->id = i;
    ca[i]->count = consume_item;
    pthread_create(&consumer_pthread_ids[i], NULL, consume, ca[i]);
  }

  for (i = 0; i < producer_count; i++) {
    pthread_join(producer_pthread_ids[i], NULL);
  }
  for (i = 0; i < consumer_count; i++) {
    pthread_join(consumer_pthread_ids[i], NULL);
  }

  for (i = 0; i < producer_count; i++) {
    free(pa[i]);
  }

  for (i = 0; i < consumer_count; i++) {
    free(ca[i]);
  }

  return 0;
}
