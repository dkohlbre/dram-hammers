#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "pagemap_read.h"

// Define to vary target pfn delta, rather than changing addr pairs
//#define VARY_DELTA
#define START_DELTA 200

// Define for using /dev/fmem, starting offset
//#define USE_FMEM
#define START_ADDR (381732*4096)

// Define when not using fmem to use malloc instead of bss
//#define USE_MALLOC

/** Configure the test here **/
#define PAGE_SIZE 4096
#define PAGES 65536*6
#define NUM_ITERS 150000000
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

unsigned long _read_pagemap(void* addr){
#ifdef USE_FMEM
  return (START_ADDR+(((uintptr_t)addr)-((uintptr_t)memory)))/PAGE_SIZE;
#else
  return read_pagemap(addr);
#endif
}


int check_memory(){
  int found;
  int i,j;
  found = 0;
  for(i=0;i<PAGES;i++){
    if(memcmp(&(memory[i*PAGE_SIZE]),mpage,PAGE_SIZE) != 0){
      printf("[Alert] Difference found! pfn #%i:\n",
             _read_pagemap(&(memory[PAGE_SIZE*i])));
      printf("[Page] ");
      for(j=0;j<PAGE_SIZE;j++){
        printf("%02X ",memory[(i*PAGE_SIZE)+j]);
      }
      printf("\n");
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
unsigned char* setup_mem(){
#ifdef USE_FMEM
  int fd = open("/dev/fmem", O_RDWR);
  if(fd <= 0){
    return NULL;
  }

  printf("[Info] mmaping /dev/fmem...\n");
  void *map = mmap(NULL, PAGES*PAGE_SIZE, PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, START_ADDR);
  return (unsigned char*)(map <= 0?NULL:map);
#elif defined(USE_MALLOC)
  printf("[Info] mallocing space...\n");
  unsigned char* a = malloc(PAGE_SIZE*PAGES);
  if(mlockall(MCL_CURRENT) < 0){
    printf("[Error] Unable to mlockall()\n");
    return NULL;
  }
  return a;

#else
  printf("[Info] Using bss...\n");
  if(mlockall(MCL_CURRENT) < 0){
    printf("[Error] Unable to mlockall()\n");
    return NULL;
  }
  return bss_memory;
#endif

}


void find_delta_run(int page_delta,void* addr1){
  // Importantly, we chose the values of X and Y so that they map to the same
  // bank, but to different rows within the bank (see paper)
  // Choose some addresses, then run the test on them
  // We want a delta of PHYS_PAGE_DELTA pages

  void *addr2;
  unsigned long long phys1,phys2;
  int j, delta;

  phys1 = _read_pagemap(addr1);
  if(phys1 <= 0){
    printf("Error getting pagemap1\n");
    exit(1);
  }
  for(j=0;j<PAGES;j++){
    addr2 = &(memory[PAGE_SIZE*j]);
    phys2 = _read_pagemap(addr2);
    if(phys2 <= 0){
      printf("Error getting pagemap2\n");
      exit(1);
    }
    if(abs(phys1 - phys2) == page_delta ){
      printf("[Info] Running with addresses(pfn): "
             "%p(%lld), %p(%lld)\n",
             addr1,phys1,addr2,phys2);
      loop(addr1,addr2,NUM_ITERS);
      check_memory();
    }
  }
}

void run_all_pairs(){
  int i;
  printf("[Info] Finding addresses %i physical frames apart\n",
         PHYS_PAGE_DELTA);
  for(i=0;i<PAGES;i++){
    find_delta_run(PHYS_PAGE_DELTA,&(memory[i*PAGE_SIZE]));
  }
}

void run_vary_delta(){
  int i;
  printf("[Info] Varying physical frames delta.\n");
  for(i=START_DELTA;i<PAGES;i++){
    find_delta_run(i,memory);
  }
}

int main(int argc, char* argv[]){
  // I hate everything
  setbuf(stdout, NULL);

  unsigned char val = 0xff;

  // Set memory (base memory and the utility page)

  memory = setup_mem();

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

  printf("[Info] Running %i iterations.\n",NUM_ITERS);

#ifdef VARY_DELTA
  run_vary_delta();
#else
  run_all_pairs();
#endif
}
