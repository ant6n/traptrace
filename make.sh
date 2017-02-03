
gcc -shared -fPIC  \
    src/override.c src/tracer.c src/helper.c src/syscallprinter.c\
    src/instructions.c src/stats.c src/syscalls.c \
    -o traptrace.so -ldl -ludis86

gcc src/main.c -o traptrace
