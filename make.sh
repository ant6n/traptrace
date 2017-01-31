
gcc -shared -fPIC  override.c tracer.c helper.c syscallprinter.c instructionprinter.c -o override.so -ldl -ludis86

