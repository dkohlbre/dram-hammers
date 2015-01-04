void loop(void* addr1, void* addr2, int iters){
  int i;
  for(i=0;i<iters;i++){
    asm volatile("mov (%0), %%rax" : : "r" (addr1) : "rax");
    asm volatile("mov (%0), %%rbx" : : "r" (addr2) : "rbx");
    asm volatile("clflush (%0)" : : "r" (addr1) : "memory");
    asm volatile("clflush (%0)" : : "r" (addr2) : "memory");
    asm volatile("mfence");
  }
}
