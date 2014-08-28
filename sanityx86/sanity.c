#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "pagemap_read.h"

void loop(void* addr1, void* addr2);

void setup_timer();
int check_memory();
void alarm_handler(int sig);

#define PAGE_SIZE 4096
#define PAGES 8192
#define SPIN_TIME 10
#define PHYS_PAGE_DELTA 2000
//Define to randomize addresses every time
#define RANDOMIZE_ADDRESS

unsigned char memory[PAGE_SIZE*PAGES];
unsigned char mpage[PAGE_SIZE];

int run;

int check_memory(){
  int found;
  int i,j;
  found = 0;
  printf("[Info] Checking all of memory array");
  for(i=0;i<PAGES;i++){
    if(!found && (i%(PAGES/4) == 0)){
      printf(".");
    }
    if(memcmp(&(memory[i*PAGE_SIZE]),mpage,PAGE_SIZE) != 0){
      printf("\n[ALERT] Difference found! Page #%i:\n",i);
      for(j=0;j<PAGE_SIZE;j++){
        printf("%02X ",memory[(i*PAGE_SIZE)+j]);
      }
      found = 1;
    }
  }
  if(!found){
    printf("\n[Info] No differences found in run #%i\n",run);
  }
  else{
    printf("[Info] Differences found in run #%i\n",run);
  }
  run++;
  return found;
}

void alarm_handler(int sig){
  // Check memory?
  if(check_memory() == 0){
    //keep running
    setup_timer();
  }
  else{
    //Done
    printf("[Info] Exiting\n");
    exit(1);
  }
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

  // Importantly, we chose the values of X and Y so that they map to the same
  // bank, but to diﬀerent rows within the bank
  // Choose some addresses
  // want a delta of PHYS_PAGE_DELTA pages
  void* addr1, *addr2;
  unsigned long long phys1,phys2;
  int i,j, delta, done=0;
  printf("[Info] Searching for good addresses");
  for(i=0;i<PAGES;i++){
    if(i%(PAGES/10) == 0)
      printf(".");
    addr1 = &(memory[PAGE_SIZE*i]);
    for(j=0;j<PAGES;j++){
      addr2 = &(memory[PAGE_SIZE*j]);

      phys1 = read_pagemap((unsigned long)addr1);
      if(phys1 <= 0){
        printf("Error getting pagemap1\n");
        exit(1);
      }
      phys2 = read_pagemap((unsigned long)addr2);
      if(phys2 <= 0){
        printf("Error getting pagemap2\n");
        exit(1);
      }
      delta = abs(phys1 - phys2) - PHYS_PAGE_DELTA;
      //      printf("DELTA: %i\n",abs(delta));
      if( delta < 10 && delta > -10){
        done = 1;
        printf("\n[Info] Phys pages: %lld %lld Delta: %i\n",phys1,phys2,delta);
        break;
      }
    }
    if(done)
      break;
  }

  if(!done){
    printf("\n");
    printf("[Info] Unable to find reasonable addresses 8MB apart\n");
    return 1;
  }

  ((char*)addr1)[0] = 'm';
  ((char*)addr2)[0] = 'm';


  // Setup the timer
  setup_timer();

  printf("[Info] Starting with addresses: 0x%08X, 0x%08X\n",addr1,addr2);
  printf("[Info] Physical Page Numbers: %lld, %lld\n",
         read_pagemap((unsigned long)addr1),read_pagemap((unsigned long)addr2));
  printf("[Info] Running tight loop for %i second intervals\n",SPIN_TIME);
  loop(addr1,addr2);

}
