#include <unistd.h>
#include <stdlib.h>
#include "helper.h"


void writeInt(int v) {
  static const int N = 20;
  char string[N];
  char* s = string + N;
  int isNegative = v < 0;
  v = abs(v);
  if (v == 0) {
    *--s = '0';
  }
  while (v > 0) {
    int d = (v % 10);
    *--s = ('0' + d);
    v = v / 10;
  }
  if (isNegative) {
    *--s = '-';
  }
  int numChars = string + N - s;
  write(STDOUT_FILENO, s, numChars);
}

// given a number, returns a char representing its lowest hex digit
static char hexDigit(int h) {
  h = h & 0xf;
  return ((h >= 10)?('a'-10):'0') + h;
}

void writeHex(int v) {
  char s[10];
  s[0] = '0'; s[1] = 'x';
  int i;
  for (i = 0; i < 8; i++) {
    int h = (v >> (i*4)) & 0xf;
    s[9-i] = hexDigit(h);
  }
  write(STDOUT_FILENO, s, 10);
}

int  strLen(const char* s) {
  int len = 0;
  while (s[len] != 0) {
    len++;
  }
  return len;
}

void writeStr(const char* s) {
  write(STDOUT_FILENO, s, strLen(s));
}

void writeStrEscaped(const char* s) {
  static char buffer[MAX_NUM_WRITE_STR_ESCAPED_CHARS*4];
  char* t = buffer;
  int numChars = 0;
  while (*s != '\0' && numChars < MAX_NUM_WRITE_STR_ESCAPED_CHARS) {
    char c = *s++;
    numChars++;
    if (c >= 32 && c <= 126) {
      *t++ = c;
    } else {
      *t++ = '\\';
      switch (c) {
      case '\n':
        *t++ = 'n';
        break;
      case '\t':
        *t++ = 't';
        break;
      default:
        *t++ = 'x';
        *t++ = hexDigit(c >> 4);
        *t++ = hexDigit(c);
        break;
      }
    }
  }
  write(STDOUT_FILENO, buffer, t - buffer);
}

