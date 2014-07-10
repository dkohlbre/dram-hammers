#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void loop(void* addr1, void* addr2);

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

  // Setup the timer
  setup_timer();
  
  void* addr1 = &(memory[PAGE_SIZE*(PAGES/5) + 50]);
  void* addr2 = &(memory[PAGE_SIZE*(PAGES/5)*3 + 50]);

  printf("[Info] Starting with addresses: 0x%08X, 0x%08X\n",addr1,addr2);
  printf("[Info] Running tight loop for %i second intervals\n",SPIN_TIME);
  loop(addr1,addr2);

}
