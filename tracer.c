#define _GNU_SOURCE 1

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "tracer.h"
#include "helper.h"
#include "syscallprinter.h"
#include "instructionprinter.h"

#define SYS_EXIT       0x01
#define SYS_EXIT_GROUP 0xfc
#define SYS_CLOSE      0x06

long long int ccycle = 0;
void trapHandler(int signo, siginfo_t *info, void *context) {
  ucontext_t *con = (ucontext_t *)context;
  uint8_t* eip = (uint8_t*)con->uc_mcontext.gregs[REG_EIP];
  ccycle++;
  if ((   eip[0] == 0xcd && eip[1] == 0x80) // int 0x80
      || (eip[0] == 0x0f && eip[1] == 0x34) // sysenter
      )  {
    int eax = con->uc_mcontext.gregs[REG_EAX];
    int ebx = con->uc_mcontext.gregs[REG_EBX];
    int ecx = con->uc_mcontext.gregs[REG_ECX];
    int edx = con->uc_mcontext.gregs[REG_EDX];
    int esi = con->uc_mcontext.gregs[REG_ESI];
    int edi = con->uc_mcontext.gregs[REG_EDI];
    
    if (eax == SYS_EXIT || eax == SYS_EXIT_GROUP) {
      writeStr("[intercepted sys-exit. cycles: ");
      writeInt(ccycle);
      writeStr("\n");
    }
    
    if (eax == SYS_CLOSE && ebx == STDOUT_FILENO) { // don't allow closing std-out
      writeStr("[skip closing std-out (");
      int offset = 0;
      // keep skipping syscall/int 0x80 instructions
      while (eip[offset] == 0xcd || (eip[offset] == 0x0f && eip[offset+1] == 0x34)) {
        writeStr(".");
        con->uc_mcontext.gregs[REG_EIP] += 2;
        offset += 2;
      }
      writeStr(")\n");
      con->uc_mcontext.gregs[REG_EAX] = 0; // return close-success
    }
    
    printSyscall(eax, ebx, ecx, edx, esi, edi);
    
  }
  print_instruction(eip);
}


void exitHandler(int signo, siginfo_t *info, void *context) {
  writeStr("exit handler, caught signal ");
  writeInt(signo);
  writeStr("\n");
  stopTrace();
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
static struct sigaction exitSa;
void startTrace() {
  init_instruction_printer();
  
  // set up trap signal handler
  trapSa.sa_flags = SA_SIGINFO;
  trapSa.sa_sigaction = trapHandler;
  sigaction(SIGTRAP, &trapSa, NULL);
  
  // set up exit signal handler
  exitSa.sa_flags = SA_SIGINFO;
  exitSa.sa_sigaction = exitHandler;
  sigaction(SIGTERM, &exitSa, NULL);
  sigaction(SIGQUIT, &exitSa, NULL);
  sigaction(SIGINT,  &exitSa, NULL);
  sigaction(SIGSTOP, &exitSa, NULL);
  sigaction(SIGHUP,  &exitSa, NULL);
  sigaction(SIGABRT, &exitSa, NULL);
  
  setTrapFlag();
}


void stopTrace() {
  clearTrapFlag();

  printf("cycles: %lld\n", ccycle);
}

