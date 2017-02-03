#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

void initialize_syscalls();
void record_syscall(void* context);
void finalize_syscalls();

#endif /* __SYSCALLS_H__ */
