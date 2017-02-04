#define _GNU_SOURCE 1

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include "tracer.h"
#include "helper.h"
#include "syscalls.h"
#include "instructions.h"
#include "stats.h"

#define SYS_EXIT       0x01
#define SYS_EXIT_GROUP 0xfc
#define SYS_CLOSE      0x06

static pid_t pid;

long long int ccycle = 0;
void trapHandler(int signo, siginfo_t *info, void *context) {
  ucontext_t *con = (ucontext_t *)context;
  uint8_t* eip = (uint8_t*)con->uc_mcontext.gregs[REG_EIP];
  ccycle++;
  
  pid_t current_pid = getpid();
  if (current_pid != pid) {
    writeStr("pid changed!\n");
    pid = current_pid;
    con->uc_mcontext.gregs[REG_EFL] &= 0xfffffeff; // reset trap flag
  }
  if ((   eip[0] == 0xcd && eip[1] == 0x80) // int 0x80
      || (eip[0] == 0x0f && eip[1] == 0x34) // sysenter
      )  {
    int eax = con->uc_mcontext.gregs[REG_EAX];
    int ebx = con->uc_mcontext.gregs[REG_EBX];
    
    if (eax == SYS_EXIT || eax == SYS_EXIT_GROUP) {
      writeStr("[intercepted sys-exit. cycles: ");
      writeInt(ccycle);
      writeStr("\n");
      finalize_stats();
      finalize_syscalls();
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
    
    record_syscall(context);
    
  }
  record_instruction(eip);
  record_stats(eip);
}


void exitHandler(int signo, siginfo_t *info, void *context) {
  stopTrace();
  writeStr("exit handler, caught signal ");
  writeInt(signo);
  writeStr("\n");
  finalize_stats();
  finalize_syscalls();
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
  pid = getpid();
  writeStr("start with PID: ");
  writeInt(pid);
  writeStr("\n");
  
  initialize_instructions();
  initialize_stats();
  initialize_syscalls();
  
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

