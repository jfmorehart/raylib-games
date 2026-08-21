#include <string.h>
#include "text.h"


void AddBufferText(TextBuffer * buffer, char * text){
    for(int i = 0; i < MAXBUFFERLENGTH; i++){
        if(buffer->cham + 1 >= MAXBUFFERLENGTH - 1){
            return;
        }
        if(text[i] == 0) return;

        buffer->charArray[buffer->cham] = text[i];
        buffer->cham++;
    }
}

void ClearBuffer(TextBuffer * buffer){
    memset(buffer->charArray, 0, MAXBUFFERLENGTH);
    buffer->cham = 0;
}