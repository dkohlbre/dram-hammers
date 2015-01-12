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

unsigned long long read_pagemap(void* virt_addr){
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
  file_offset = ((unsigned long long)virt_addr) / getpagesize() * PAGEMAP_ENTRY;
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
      return 0;
    }
    c_buf[PAGEMAP_ENTRY - i - 1] = c;
  }
  for(i=0; i < PAGEMAP_ENTRY; i++){
    read_val = (read_val << 8) + c_buf[i];
  }
  if(GET_BIT(read_val, 63))
    return (unsigned long long) GET_PFN(read_val);
  else
    return 0;

  fseek(f,0,SEEK_SET);
  return 0;
}
