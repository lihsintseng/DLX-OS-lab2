#ifndef __USERPROG__
#define __USERPROG__
typedef unsigned char uint8;
typedef struct circular_queue {
//  int numprocs;
  uint8 head;
  uint8 tail;
  char buffer [BUFFERSIZE];
} circular_queue;

#define PRODUCER_TO_RUN "producer.dlx.obj"
#define CONSUMER_TO_RUN "consumer.dlx.obj"

#endif
