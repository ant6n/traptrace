
gcc -shared -fPIC  override.c tracer.c helper.c syscallprinter.c -o override.so -ldl

