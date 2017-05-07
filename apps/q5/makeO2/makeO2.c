#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "spawn.h"

void main (int argc, char *argv[])
{
  atom *at;                // Used to access missile codes in shared memory page
 // uint32 h_mem;          // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_t s_procs2_completed; // Semaphore to signal the original process that we're done
  lock_t l_buffer_lock;    // Lock the buffer
  cond_t c_O2;
  cond_t c_O;
  
  
  char flag;
  char i;

  if (argc != 7) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  #ifdef DEBUG
    Printf("Created one Make_O2 process\n");
  #endif


  // Convert the command-line strings into integers for use as handles
 // h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  c_O2 = dstrtol(argv[2], NULL, 10);
  s_procs_completed = dstrtol(argv[3], NULL, 10);
  s_procs2_completed = dstrtol(argv[4], NULL, 10);
  l_buffer_lock = dstrtol(argv[5], NULL, 10);
  c_O = dstrtol(argv[6], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((at = (atom *)shmat(dstrtol(argv[1], NULL, 10))) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }


  if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad acquire lock l_buffer_lock(%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }
  




  while(at->O < 2){
    cond_wait(c_O);

    if(at->endO_flag){
      if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
        Printf("Bad release lock l_buffer_lock (%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
      }
      if(sem_signal(s_procs2_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs2_completed (%d) in ", s_procs2_completed); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
      Exit();
       
    }
  }



  at->O-=2;
  at->O2++;
  Printf("O + O -> O2\n");
  
  cond_signal(c_O2);
  
  if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad release lock l_buffer_lock(%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }
     
  if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad acquire lock l_buffer_lock(%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }
  
  if(at->O2 >= 3 && at->temp >= 50){
    at->O3+=2;
    at->O2-=3;
    #ifdef DEBUG
      Printf("We have %d O2, %d O3, now.\n", at->O2, at->O3);
    #endif
    Printf("3O2 -> 2O3\n");
  
  } 
  if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad release lock l_buffer_lock(%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }

  // Now print a message to show that everything worked
  #ifdef DEBUG
//  Printf("Producer: This is one of the %d producers you created.  \n", at->numprocs);
  //Printf("spawn_me: My PID is %d\n", Getpid());

  // Signal the semaphore to tell the original process that we're done
  Printf("spawn_me: makeO2 PID %d is complete.\n", Getpid());
  #endif



  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  #ifdef DEBUG
    Printf("MakeO2 Finish!!! \n");
  #endif
}
