#ifndef __USERPROG__
#define __USERPROG__


typedef struct circular_queue {
  int numprocs;
  int head;
  int tail;
  char buffer [BUFFERSIZE];
} circular_queue;

#define PRODUCER_TO_RUN "producer.dlx.obj"
#define CONSUMER_TO_RUN "consumer.dlx.obj"

#endif
