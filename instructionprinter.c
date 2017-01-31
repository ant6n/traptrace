#include "instructionprinter.h"
#include <udis86.h>
#include "helper.h"
#include <unistd.h>

static ud_t ud;


void init_instruction_printer() {
  ud_init(&ud);
  ud_set_mode(&ud, 32);
  ud_set_syntax(&ud, UD_SYN_INTEL);
}

void print_instruction(void* eip) {
  fwrite_instruction(STDOUT_FILENO, eip, 1);
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
