#include <udis86.h>
#include "helper.h"


static ud_t ud;


void init_instruction_printer() {
  ud_init(&ud);
  ud_set_mode(&ud, 32);
  ud_set_syntax(&ud, UD_SYN_INTEL);
}

void print_instruction(void* eip) {
  ud_set_input_buffer(&ud, eip, 16);
  int numBytes = ud_disassemble(&ud);
  if (0) {
    writeHex((long)eip);
    writeStr(": ");
    writeStr(ud_insn_asm(&ud));
    writeStr("  (");
    writeStr(ud_insn_hex(&ud));
    //writeInt(numBytes);
    writeStr(")\n");
  }
}
