
gcc -shared -fPIC  override.c tracer.c helper.c syscallprinter.c instructions.c stats.c syscalls.c -o override.so -ldl -ludis86

