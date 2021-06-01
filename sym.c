#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "struct/unix_list.h"
#include "struct/my_ALU.h"
#include "Operation.h"

int main(){
    int i;
    char* ans;
    char* a[10] = {"(\0", "2\0", "^\0", "23\0", ")\0", 0};
    init_count();
    for (i = 0; a[i] != 0; i++){
        ans = count_elem(a[i]);
    }

    printf("%s", ans);
    return 0;
}
