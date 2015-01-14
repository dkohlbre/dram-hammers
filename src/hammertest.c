#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "pagemap_read.h"


// Define for using /dev/fmem
#define USE_FMEM
// Defined when not using fmem to use malloc instead of bss
//#define USE_MALLOC

/** Configure the test here **/
#define PAGE_SIZE 4096
#define PAGES 65536
#define NUM_ITERS 100000000
#define PHYS_PAGE_DELTA 2000

// Memory array for testing
unsigned char *memory;
#if !defined(USE_FMEM) && !defined(USE_MALLOC)
unsigned char bss_memory[PAGE_SIZE*PAGES];
#endif


// Utility page holding our data pattern
unsigned char mpage[PAGE_SIZE];

// Current run number
int run;

void loop(void* addr1, void* addr2, int iterations);
int check_memory();

int check_memory(){
  int found;
  int i,j;
  found = 0;
  for(i=0;i<PAGES;i++){
    if(memcmp(&(memory[i*PAGE_SIZE]),mpage,PAGE_SIZE) != 0){
      printf("[ALERT] Difference found! Phyiscal page #%i:\n",
             read_pagemap(&(memory[PAGE_SIZE*i])));
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



// Grab an allocation via /dev/fmem
// Requires using the modified fmem kmodule
// start_addr defines an optional starting physical address number
unsigned char* setup_mem(unsigned long long start_addr){
#ifdef USE_FMEM
  int fd = open("/dev/fmem", O_RDWR);
  if(fd <= 0){
    return NULL;
  }

  printf("[Info] mmaping /dev/fmem...\n");
  void *map = mmap(NULL, PAGES*PAGE_SIZE, PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, start_addr);
  return (unsigned char*)(map <= 0?NULL:map);
#elif defined(USE_MALLOC)
  printf("[Info] mallocing space...\n");
  return malloc(PAGE_SIZE*PAGES);
#else
  printf("[Info] Using bss...\n");
  return bss_memory;
#endif

}

unsigned long _read_pagemap(void* addr){
#ifdef USE_FMEM
  return (((uintptr_t)memory)-((uintptr_t)addr))/PAGE_SIZE;
#else
  return read_pagemap(addr);
#endif
}

int main(int argc, char* argv[]){
  // I hate everything
  setbuf(stdout, NULL);

  unsigned char val = 'm';

  // Set memory (base memory and the utility page)

  unsigned long long start_addr = 0;
  memory = setup_mem(start_addr);

  if(memory == NULL){
    printf("[Error] Unable to get valid memory!\n");
    exit(1);
  }
  memset(memory,val,PAGE_SIZE*PAGES);
  memset(mpage,val,PAGE_SIZE);

  run = 0;

  printf("[Info] Memory starts at %p, pfn %i.\n"
         "[Info] Set as %iMB byte region, value of 0x%02X.\n",
         memory,_read_pagemap(memory),(PAGE_SIZE*PAGES)/(1024*1024),val);
  printf("[Info] Finding addresses %i physical frames apart and "
         "running %i iterations on them.\n",PHYS_PAGE_DELTA,NUM_ITERS);

  // Importantly, we chose the values of X and Y so that they map to the same
  // bank, but to different rows within the bank (see paper)
  // Choose some addresses, then run the test on them
  // We want a delta of PHYS_PAGE_DELTA pages
  void* addr1, *addr2;
  unsigned long long phys1,phys2;
  int i,j, delta, done=0;
  for(i=0;i<PAGES;i++){
    addr1 = &(memory[PAGE_SIZE*i]);
    for(j=0;j<PAGES;j++){
      addr2 = &(memory[PAGE_SIZE*j]);
      phys1 = _read_pagemap(addr1);
      if(phys1 <= 0){
        printf("Error getting pagemap1\n");
        exit(1);
      }
      phys2 = _read_pagemap(addr2);
      if(phys2 <= 0){
        printf("Error getting pagemap2\n");
        exit(1);
      }
      if(abs(phys1 - phys2) == PHYS_PAGE_DELTA ){
        printf("[Info] Running with addresses(pfn): "
               "%p(%lld), %p(%lld)\n",
               addr1,phys1,addr2,phys2);
        loop(addr1,addr2,NUM_ITERS);
        check_memory();
      }
    }
  }

}
