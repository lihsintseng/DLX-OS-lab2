#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "spawn.h"

void main (int argc, char *argv[])
{
  circular_queue *mc;        // Used to access missile codes in shared memory page
 // uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  lock_t l_buffer_lock;    // Lock the buffer
  char flag;
  char message[] = "Hello World";
  char i;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
 // h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  l_buffer_lock = dstrtol(argv[3], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((mc = (circular_queue *)shmat(dstrtol(argv[1], NULL, 10))) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }


  for(i = 0; i < 11; i++){
    flag = 0;
    while(flag == 0){
      if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
        Printf("Bad acquire lock s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
      }
      if((mc->head + 1) % (BUFFERSIZE) != mc->tail){
        flag = 1;
        mc->buffer[mc->head] = message[i];
        mc->head = (mc->head +1 ) % (BUFFERSIZE);
        Printf("Producer %d inserted: %c\n", Getpid(), message[i]);
      }
      if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
        Printf("Bad release lock s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
      }
    }
  }

  // Now print a message to show that everything worked
  #ifdef DEBUG
//  Printf("Producer: This is one of the %d producers you created.  \n", mc->numprocs);
  //Printf("spawn_me: My PID is %d\n", Getpid());

  // Signal the semaphore to tell the original process that we're done
  Printf("Producer: PID %d is complete.\n", Getpid());
  #endif
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
