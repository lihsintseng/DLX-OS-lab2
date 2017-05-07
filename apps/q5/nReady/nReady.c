#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "spawn.h"

void main (int argc, char *argv[])
{
  atom *at;                // Used to access missile codes in shared memory page
 // uint32 h_mem;          // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_t s_child_completed; // Semaphore to signal the child process that we're done
  sem_t s_child2_completed; // Semaphore to signal the child process that we're done
  lock_t l_buffer_lock;    // Lock the buffer
  cond_t c_O2;
  char s_child_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char s_child2_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  cond_t c_N;
  
  char flag;
  char message[] = "Hello World";
  char i;
  char c_N_str [10];

  if (argc != 5) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 


  if ((c_N = cond_create(l_buffer_lock)) == SYNC_FAIL) {
    Printf("Bad c_N in "); Printf(argv[0]); Printf("\n");
    Exit();
  }


  // Convert the command-line strings into integers for use as handles
 // h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  c_O2 = dstrtol(argv[2], NULL, 10);
  s_procs_completed = dstrtol(argv[3], NULL, 10);
  l_buffer_lock = dstrtol(argv[4], NULL, 10);

  ditoa(c_N, c_N_str);
  // Map shared memory page into this process's memory space
  if ((at = (atom *)shmat(dstrtol(argv[1], NULL, 10))) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  #ifdef DEBUG
    Printf("How many child: %d %d\n", at->totalO, -(at->totalN / 2 -1));
  #endif
  if ((s_child_completed = sem_create(-(at->totalN / 2 -1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((s_child2_completed = sem_create(-(at->totalN - at->totalN / 2 -1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  ditoa(s_child_completed, s_child_completed_str);
  ditoa(s_child2_completed, s_child2_completed_str);

  for(i = 0; i < at->totalN; i++){
    process_create(MAKE_N2, argv[1], argv[2], s_child_completed_str, s_child2_completed_str, argv[4], c_N_str, NULL);
    if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
      Printf("Bad acquire lock s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    }
    at->N++;
    Printf("%d N\n", at->totalN - i - 1);
    cond_signal(c_N);
      
    if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
      Printf("Bad release lock s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    }
  }

  if (sem_wait(s_child_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_child_completed (%d) in ", s_child_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  lock_acquire(l_buffer_lock);
  at->endN_flag = 1;
  cond_broadcast(c_N);
  lock_release(l_buffer_lock);
  // Now print a message to show that everything worked
  #ifdef DEBUG
//  Printf("Producer: This is one of the %d producers you created.  \n", at->numprocs);
  //Printf("spawn_me: My PID is %d\n", Getpid());

  // Signal the semaphore to tell the original process that we're done
  Printf("spawn_me: nReady PID %d is complete.\n", Getpid());
  #endif

  if (sem_wait(s_child2_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_child2_completed (%d) in ", s_child2_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
  //at->endN_flag = 1;
  //cond_broadcast(c_N);
}
