void loop(void* addr1, void* addr2, int iters){
  int i;
  for(i=0;i<iters;i++){
    asm volatile("movl (%0), %%eax" : : "r" (addr1) : "eax");
    asm volatile("movl (%0), %%ebx" : : "r" (addr2) : "ebx");
    asm volatile("clflush (%0)" : : "r" (addr1) : "memory");
    asm volatile("clflush (%0)" : : "r" (addr2) : "memory");
    asm volatile("mfence");
  }
}
