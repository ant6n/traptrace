#include <stdint.h>
#include "helper.h"
#include <fcntl.h>
#include <unistd.h>
#include "instructionprinter.h"


#define I16 (65536)
static long long count_per_instruction[I16];


void initialize_stats() {
  for (int i = 0; i < I16; i++) {
    count_per_instruction[i] = 0;
  }
}


void record_stats(void* eip) {
  uint8_t* ip = (uint8_t*)eip;
  uint16_t instruction = (((uint16_t)ip[0]) << 8) + (((uint16_t)ip[1]) << 0);
  count_per_instruction[instruction]++;
}


void write_stats() {
  uint8_t array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  writeStr("write on exit\n");
  int f = open("stats.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
  if (f < 0) {
    writeStr("open failed\n");
  }
  for (int i = 0; i < I16; i++) {
    if (count_per_instruction[i]) {
      fwriteHex(f, i);
      fwriteStr(f, ": ");
      fwriteInt(f, (int)count_per_instruction[i]);
      fwriteStr(f, "  ");
      
      array[0] = i >> 8;
      array[1] = i >> 0;
      fwrite_instruction(f, array, 0);
      
      //fwriteStr(f, "\n");
    }
  }
  close(f);
}
