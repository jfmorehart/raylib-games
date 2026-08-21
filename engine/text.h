#include <string.h>

#define MAXBUFFERLENGTH 1024
typedef struct TextBuffer{
    char charArray[MAXBUFFERLENGTH];
    int cham;
}   TextBuffer;

void ClearBuffer(TextBuffer * buffer);

void AddBufferText(TextBuffer * buffer, char * text);