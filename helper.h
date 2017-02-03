#ifndef __HELPER_H__
#define __HELPER_H__

void fwriteInt(int f, int v);
void fwriteHex(int f, int v);
void fwriteStr(int f, const char* s);

int  strLen(const char* s);

void writeInt(int v);
void writeHex(int v);
void writeStr(const char* s);

// write string while escaping it, prints at most max num chars
void fwriteStrEscaped(int f, const char* s);
#define MAX_NUM_WRITE_STR_ESCAPED_CHARS 200

// given a filename or '-', opens the file for writing or returns STDOUT_FILENO
int open_or_stdout(char* filename);

#endif /* __HELPER_H__ */

