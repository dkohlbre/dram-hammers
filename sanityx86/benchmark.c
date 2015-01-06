#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


unsigned int prev_ct;
unsigned int counter;

void bench(int* counter,void* addr1, void* addr2);
void setup_timer();
int check_memory();
void alarm_handler(int sig);

#define PAGE_SIZE 4096
#define PAGES 8192
#define SPIN_TIME 10

//Define to randomize addresses every time
#define RANDOMIZE_ADDRESS

unsigned char memory[PAGE_SIZE*PAGES];
unsigned char mpage[PAGE_SIZE];

int run;

void alarm_handler(int sig){
  //  printf("[Info] %i loop iteration count.\n",counter);
  printf("[Info] %i iterations in the last %i seconds.\n",(counter-prev_ct),SPIN_TIME);
  prev_ct = counter;
  setup_timer();
}

void setup_timer(){
  signal(SIGALRM,alarm_handler);
  alarm(SPIN_TIME);
}

int main(int argc, char* argv[]){
  // I hate everything
  setbuf(stdout, NULL);

  // Set memory
  memset(memory,'m',PAGE_SIZE*PAGES);
  memset(mpage,'m',PAGE_SIZE);

  run = 0;
  prev_ct = 0;
  counter = 0;

  // Setup the timer
  setup_timer();

  // Choose some random addresses for benchmarking purposes
  void* addr1 = &(memory[PAGE_SIZE*(PAGES/5) + 50]);
  void* addr2 = &(memory[PAGE_SIZE*(PAGES/5)*3 + 50]);

  printf("[Info] Starting with addresses: 0x%08X, 0x%08X\n",addr1,addr2);
  printf("[Info] Running tight loop for %i second intervals\n",SPIN_TIME);
  bench(&counter,addr1,addr2);

}
