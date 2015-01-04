#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "pagemap_read.h"

void loop(void* addr1, void* addr2, int iterations);

void setup_timer();
int check_memory();
void alarm_handler(int sig);

#define PAGE_SIZE 4096
#define PAGES 8192
#define NUM_ITERS 10000000
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
  //  printf("[Info] Checking all of memory array");
  for(i=0;i<PAGES;i++){
    if(memcmp(&(memory[i*PAGE_SIZE]),mpage,PAGE_SIZE) != 0){
      printf("[ALERT] Difference found! Page #%i:\n",i);
      for(j=0;j<PAGE_SIZE;j++){
        printf("%02X ",memory[(i*PAGE_SIZE)+j]);
      }
      found = 1;
    }
  }
  if(!found){
    printf("[Info] No differences found in run #%i\n----\n",run);
  }
  else{
    printf("[Info] Differences found in run #%i\n====\n",run);
  }
  run++;
  return found;
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
  //  printf("[Info] Searching for good addresses");
  for(i=0;i<PAGES;i++){
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
      if( delta == 0){

        printf("[Info] %i iterations with addresses: 0x%08X, 0x%08X\n",
               NUM_ITERS,addr1,addr2);
        printf("[Info] Physical Page Numbers: %lld, %lld\n",phys1,phys2);
        loop(addr1,addr2,NUM_ITERS);
        check_memory();
      }
    }
  }

}
