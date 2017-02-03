#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__


void initialize_instructions();
void record_instruction(void* eip);
void fwrite_instruction(int f, void* eip, int print_address);
void finalize_instructions();

#endif /* __INSTRUCTIONS_H__ */
