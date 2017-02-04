#define _GNU_SOURCE 1

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "syscallprinter.h"
#include "main.h"
#include "helper.h"


static int f; // negative number means don't record

void initialize_syscalls() {
  char* filename = getenv(SYSCALLS_FILE);
  
  if (filename == NULL) {
    f = -1;
    return;
  }

  writeStr("[record syscalls to: ");
  writeStr(filename);
  writeStr("]\n");
  
  f = open_or_stdout(filename);
  if (f < 0) {
    writeStr("opening syscall file: '");
    writeStr(filename);
    writeStr("' failed.");
  }
}

void record_syscall(void* context) {
  if (f < 0) return;
  
  ucontext_t *con = (ucontext_t *)context;
  uint8_t* eip = (uint8_t*)con->uc_mcontext.gregs[REG_EIP];
  if ((   eip[0] == 0xcd && eip[1] == 0x80) // int 0x80
      || (eip[0] == 0x0f && eip[1] == 0x34) // sysenter
      )  {
    int eax = con->uc_mcontext.gregs[REG_EAX];
    int ebx = con->uc_mcontext.gregs[REG_EBX];
    int ecx = con->uc_mcontext.gregs[REG_ECX];
    int edx = con->uc_mcontext.gregs[REG_EDX];
    int esi = con->uc_mcontext.gregs[REG_ESI];
    int edi = con->uc_mcontext.gregs[REG_EDI];
    
    fwrite_syscall(f, eax, ebx, ecx, edx, esi, edi);
  }
}

void finalize_syscalls() {
  if ((f >= 0) && (f != STDOUT_FILENO)) {
    close(f);
  }
}
