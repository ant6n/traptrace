#ifndef __HELPER_H__
#define __HELPER_H__

void writeInt(int v);
void writeHex(int v);
int  strLen(const char* s);
void writeStr(const char* s);

// write string while escaping it, prints at most max num chars
void writeStrEscaped(const char* s);
#define MAX_NUM_WRITE_STR_ESCAPED_CHARS 200

#endif /* __HELPER_H__ */

