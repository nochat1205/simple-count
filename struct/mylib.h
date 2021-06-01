#ifndef MYLIB_H
#define MYLIB_H

#include <stdlib.h>
#include <string.h>

#define mylib_capacity_add 12

int isNumber(char ch);
int isNumeric(char *str);

int isNumber(char ch){
    return  '0' <= ch && ch <= '9';
}

int isNumeric(char *str){
    // -号开头 有无点号
    int i;
    if(*str == '-') ++str;
    for(i = 0; str[i] != '\0'; ++i){
        if(str[i] == '.' || isNumber(str[i])) continue;
        else return 0;
    }
    return 1;
}

unsigned int my_strcat(char *str1, char* str2, unsigned int str_max_size){
    int len = strlen(str1) + strlen(str2);
    if(len >= str_max_size-1)
        str1 = (char *)realloc(str1, sizeof(char)*(len+mylib_capacity_add));
    strcpy(str1, str2);
    return len+mylib_capacity_add;
}

#endif
