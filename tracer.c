#define _GNU_SOURCE 1

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "tracer.h"

#define SYS_EXIT       0x01
#define SYS_EXIT_GROUP 0xfc
#define SYS_CLOSE      0x06

void writeInt(int v) {
  char s[10];
  s[0] = '0'; s[1] = 'x';
  int i;
  for (i = 0; i < 8; i++) {
    int h = (v >> (i*4)) & 0xf;
    s[9-i] = ((h >= 10)?('a'-10):'0') + h;
  }
  write(STDOUT_FILENO, s, 10);
}

void writeStr(char* s) {
  int len = 0;
  while (s[len] != 0) {
    len++;
  }
  write(STDOUT_FILENO, s, len);
}

long long int ccycle = 0;
void trapHandler(int signo, siginfo_t *info, void *context) {
  ucontext_t *con = (ucontext_t *)context;
  uint8_t* eip = (uint8_t*)con->uc_mcontext.gregs[REG_EIP];
  ccycle++;
  // if instruction is interrupt or sysenter
  if (0xcd == eip[0] || (eip[0] == 0x0f && eip[1] == 0x34)) {
    uint8_t code = eip[1];
    int eax = con->uc_mcontext.gregs[REG_EAX];
    int ebx = con->uc_mcontext.gregs[REG_EBX];
    if (eax == SYS_EXIT || eax == SYS_EXIT_GROUP) {
      writeStr("intercepted sys exit. cycles:");
      writeInt(ccycle);
      writeStr("\n");
    }
    if (eax == SYS_CLOSE && ebx == STDOUT_FILENO) { // don't allow closing std-out
      //writeStr("skip closing std-out\n");
      int offset = 0;
      // keep skipping syscall/int 0x80 instructions
      while (eip[offset] == 0xcd || (eip[offset] == 0x0f && eip[offset+1] == 0x34)) {
        //writeStr("skip\n");
        con->uc_mcontext.gregs[REG_EIP] += 2;
        offset += 2;
      }
    }
  }
}

void setTrapFlag() {
  // set trap flag
  asm volatile("pushfl\n"
               "orl $0x100, (%esp)\n"
               "popfl\n"
               );
}

void clearTrapFlag() {
  // clear trap flag
  asm volatile("pushfl\n"
               "andl $0xfffffeff, (%esp)\n"
               "popfl\n"
               );
}


static struct sigaction trapSa;
void startTrace() {
  // set up trap signal handler
  trapSa.sa_flags = SA_SIGINFO;
  trapSa.sa_sigaction = trapHandler;
  sigaction(SIGTRAP, &trapSa, NULL);
    
  setTrapFlag();
}

