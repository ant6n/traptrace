#define _GNU_SOURCE
#include <dlfcn.h>
#include <unistd.h>
#include "tracer.h"
#include "helper.h"
#include "main.h"


typedef int (*LibcStartMainFnType)(int (*main)(int, char **, char **), int argc, char **argv,
                                   int (*init)(void), void (*fini)(void),
                                   void (*ldso_fini)(void), void (*stack_end));

int __libc_start_main(int (*main)(int, char **, char **), int argc, char **argv,
                      int (*init)(void), void (*fini)(void),
                      void (*ldso_fini)(void), void (*stack_end)) {
  LibcStartMainFnType orig_main = (LibcStartMainFnType)dlsym(RTLD_NEXT, LIBC_START_MAIN);
  writeStr("[intercepted main]\n");
  startTrace();
  int result = orig_main(main, argc, argv, init, fini, ldso_fini, stack_end);
  write(STDOUT_FILENO, "after main\n", 11);
  return result;
}
