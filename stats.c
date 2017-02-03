#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "helper.h"
#include "instructions.h"
#include "main.h"


#define I16 (65536)
static long long count_per_instruction[I16];
static char* filename; // NULL means don't record

void initialize_stats() {
  filename = getenv(STATS_FILE);
  if (filename == NULL) return;
  
  for (int i = 0; i < I16; i++) {
    count_per_instruction[i] = 0;
  }
  
  writeStr("record stats to: ");
  writeStr(filename);
  writeStr("\n");
}


void record_stats(void* eip) {
  if (!filename) return;
  
  uint8_t* ip = (uint8_t*)eip;
  uint16_t instruction = (((uint16_t)ip[0]) << 8) + (((uint16_t)ip[1]) << 0);
  count_per_instruction[instruction]++;
}


void finalize_stats() {
  if (!filename) return;
  
  uint8_t array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  writeStr("write stats to '");
  writeStr(filename);
  writeStr("'\n");

  int f = open_or_stdout(filename);
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
    }
  }
  if (f != STDOUT_FILENO) {
    close(f);
  }
}
