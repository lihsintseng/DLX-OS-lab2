#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  int numN = 0;                   // Used to store number of atom N to create
  int numO = 0;                   // Used to store number of atom O to create
  int temp = 0;                   // temperature;
  int i;                          // Loop index variable
  atom *at;                       // Used to get address of shared memory page
  uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t s_n_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  sem_t s_o_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  lock_t l_buffer_lock;           // Lock the buffer
  cond_t c_O2;                    // Wait O2
  char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_n_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char s_o_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char l_buffer_lock_str[10];     // Used as command-line argument to pass page_mapped handle locks
  char c_O2_str[10];       // Used as command-line argument to pass page_mapped handle condition variable
  int tmp;
  if (argc != 4) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of processes to create>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  numN = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  #ifdef DEBUG
  Printf("Creating %d processes\n", numN);
  #endif
  numO = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  #ifdef DEBUG
  Printf("Creating %d processes\n", numO);
  #endif
  temp = dstrtol(argv[3], NULL, 10);
  // Allocate space for a shared memory page, which is exactly 64KB
  // Note that it doesn't matter how much memory we actually need: we 
  // always get 64KB
  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Map shared memory page into this process's memory space
  if ((at = (atom*)shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // Put some values in the shared memory, to be read by other processes
  at->N = 0;
  at->O = 0;
  at->N2 = 0;
  at->O2 = 0;
  at->AN = 0;
  at->WN = 0;
  //at->curO = 0;
  at->end_flag = 0;
  at->totalN = numN;
  at->totalO = numO;
  at->endN_flag = 0;
  at->endO_flag = 0;
  at->temp = temp;

  #ifdef DEBUG
    Printf("How many child: %d %d\n", at->totalO, at->totalN);
  #endif
  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_n_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }
  if ((s_o_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  if ((l_buffer_lock = lock_create()) == SYNC_FAIL) {
    Printf("Bad lock_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  if ((c_O2 = cond_create(l_buffer_lock)) == SYNC_FAIL) {
    Printf("Bad c_O2 in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Setup the command-line arguments for the new process.  We're going to
  // pass the handles to the shared memory page and the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(h_mem, h_mem_str);
  ditoa(s_n_procs_completed, s_n_procs_completed_str);
  ditoa(s_o_procs_completed, s_o_procs_completed_str);
  ditoa(l_buffer_lock, l_buffer_lock_str);
  ditoa(c_O2, c_O2_str);

  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.
//  for(i=0; i<numN; i++) {
    process_create(NREADY_TO_RUN, h_mem_str, c_O2_str, s_n_procs_completed_str, l_buffer_lock_str, NULL);
    #ifdef DEBUG
    Printf("Nprocess %d created\n", i);
    #endif
  //}
//  for(i=0; i<numO; i++) {
    process_create(OREADY_TO_RUN, h_mem_str, c_O2_str, s_o_procs_completed_str, l_buffer_lock_str, NULL);
    #ifdef DEBUG
    Printf("Oprocess %d created\n", i);
    #endif
  //}
  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_o_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_o_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad acquire lock l_buffer_lock (%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }
 
  at->end_flag = 1;
  
  while(at->WN != 0){ 
    cond_broadcast(c_O2);
    if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
      Printf("Bad release lock l_buffer_lock (%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
    }
    if(lock_acquire(l_buffer_lock) != SYNC_SUCCESS){
      Printf("Bad acquire lock l_buffer_lock (%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
    }

  }

  if(lock_release(l_buffer_lock) != SYNC_SUCCESS){
    Printf("Bad release lock l_buffer_lock (%d) in ", l_buffer_lock); Printf(argv[0]); Printf(", exiting...\n");
  }

  
  

  if (sem_wait(s_n_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_n_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }


  #ifdef DEBUG
  Printf("We have %d O, %d O2, %d O3.\n", at->O, at->O2, at->O3);
  Printf("and we also have %d N, %d N2, %d NO2.\n", at->N, at->N2, at->NO2);
  Printf("All other processes completed, exiting main process.\n");
  #endif
}
