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
void writeStrEscaped(const char* s);
#define MAX_NUM_WRITE_STR_ESCAPED_CHARS 200

#endif /* __HELPER_H__ */

