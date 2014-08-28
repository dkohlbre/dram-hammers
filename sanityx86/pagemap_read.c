#include "pagemap_read.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (X & ((uint64_t)1<<Y)) >> Y
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF

FILE* f;

unsigned long long read_pagemap(unsigned long virt_addr){
  int i, c, status;
  uint64_t read_val, file_offset;
  if(!f){
    f = fopen("/proc/self/pagemap","rb");
  }
  if(!f){
    printf("Pagemap Read Error! Cannot open\n");
    return 0;
  }

  //Shifting by virt-addr-offset number of bytes
  //and multiplying by the size of an address (the size of an entry in pagemap file)
  file_offset = virt_addr / getpagesize() * PAGEMAP_ENTRY;
  //printf("Vaddr: 0x%lx, Page_size: %d, Entry_size: %d\n", virt_addr, getpagesize(), PAGEMAP_ENTRY);
  //printf("Reading %s at 0x%llx\n", path_buf, (unsigned long long) file_offset);
  status = fseek(f, file_offset, SEEK_SET);
  if(status){
    perror("Failed to do fseek!");
    return 0;
  }
  errno = 0;
  read_val = 0;
  unsigned char c_buf[PAGEMAP_ENTRY];
  for(i=0; i < PAGEMAP_ENTRY; i++){
    c = getc(f);
    if(c==EOF){
      //      printf("\nReached end of the file\n");
      return 0;
    }
    c_buf[PAGEMAP_ENTRY - i - 1] = c;
    //    printf("[%d]0x%x ", i, c);
  }
  for(i=0; i < PAGEMAP_ENTRY; i++){
    //printf("%d ",c_buf[i]);
    read_val = (read_val << 8) + c_buf[i];
  }
  //  printf("\n");
  //  printf("Result: 0x%llx\n", (unsigned long long) read_val);
  //if(GET_BIT(read_val, 63))
  if(GET_BIT(read_val, 63))
    return (unsigned long long) GET_PFN(read_val);
  //    printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
  else
    return 0;
  //printf("Page not present\n");

  fseek(f,0,SEEK_SET);
  return 0;
}
