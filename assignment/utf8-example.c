#include <stdio.h>      /* printf */
#include "../src/Helpers/UTF8/utf8.h"       /* u8_ functions */
#include "../src/Helpers/UTF8/accents.h"
#include <stddef.h>

int main(){
    char str[]="België ïś éèñ lánd\0";
    char buf1[10];
    char buf2[10];
    int i = 0,j = 0;
    wchar_t c;
    while((c = u8_nextchar(str,&i))){
        u8_wc_toutf8(buf1,c);
        uint32_t norm = remove_accent(c);
        u8_wc_toutf8(buf2,norm);
        printf("%5d %s (%4d,%s) is first in '%s' (bytes %d-%d)\n", c,buf1, norm, buf2 , str+j, j, i);
        j = i;
    }
}
