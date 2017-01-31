#ifndef __PRINT_INSTRUCTION_H__
#define __PRINT_INSTRUCTION_H__


void init_instruction_printer();
void print_instruction(void* eip);
void fwrite_instruction(int f, void* eip, int print_address);

#endif /* __PRINT_INSTRUCTION_H__ */
