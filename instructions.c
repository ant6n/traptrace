#include <unistd.h>
#include <udis86.h>
#include <stdlib.h>
#include "instructions.h"
#include "helper.h"
#include "main.h"


static ud_t ud;
static int f; // negative number means don't record

void initialize_instructions() {
  ud_init(&ud);
  ud_set_mode(&ud, 32);
  ud_set_syntax(&ud, UD_SYN_INTEL);
  
  char* filename = getenv(INSTRUCTIONS_FILE);
  if (filename == NULL) {
    f = -1;
  } else {
    writeStr("record instructinos to: ");
    writeStr(filename);
    writeStr("\n");
    
    f = open_or_stdout(filename);
    if (f < 0) {
      writeStr("opening syscall file: '");
      writeStr(filename);
      writeStr("' failed.");
    }
  }
}

void record_instruction(void* eip) {
  if (f >= 0) {
    fwrite_instruction(f, eip, 1);
  }
}

void fwrite_instruction(int f, void* eip, int print_address) {
  ud_set_input_buffer(&ud, eip, 16);
  int numBytes = ud_disassemble(&ud);
  if (1) {
    if (print_address) {
      fwriteHex(f, (long)eip);
      fwriteStr(f, ": ");
    }
    fwriteStr(f, ud_insn_asm(&ud));
    fwriteStr(f, "  (");
    fwriteStr(f, ud_insn_hex(&ud));
    fwriteStr(f, ")\n");
  }
}

void finalize_instructions() {
  if ((f >= 0) && (f != STDOUT_FILENO)) {
    close(f);
  }

}
