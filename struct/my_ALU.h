#ifndef _MY_ALU_H_
#define _MY_ALU_H_
#include <math.h>
#include <string.h>
#include "mylib.h"
#define POINT 0b01111111
#define CAP 100
#define END 101

// 表示位数 -99, 99
// . 125 01111111

char* add(char*, char*);
char* chengfa(char*, char*); // 向上依赖, 不存在向下依赖
char* reciprocal(char* a);
char* accuracy_decoration(char *a, int accuracy);
char isPoint(char ch);
int number_len(char* a);
int find_point(char* a);
int first_efective(char* a);  // 首个有效位
void digit_add(char*, int);
void take_negative(char *a);
char* initLongNumerical(int len);
void my_itoa_10(int num, char* ch, int len);
void show(char *a);

char* add(char* a, char* b){
    int i, j;
    int a_p, b_p, acc_p;
    char* acc;
    int a_len = number_len(a), b_len = number_len(b);
    int acc_len;

    /* 简单粗暴处理 */
    /* 寻找浮点 */
    a_p = find_point(a);
    b_p = find_point(b);

    /* 运算预处理 结构体化处理? */
    acc_p = __max(a_p, b_p)+1;
    acc_len = acc_p + __max(a_len-a_p, b_len-b_p);
    acc = initLongNumerical(acc_len);
    acc[acc_p] = POINT;

    /* 对阶->运算 */
    for(i=acc_len-1; i >= 0; --i){
        int offset = i-acc_p;
        if(offset == 0) continue;

        int a_abs = offset + a_p;
        int b_abs = offset + b_p;

        if(a_abs >= 0 && a_abs < a_len)
            digit_add(acc+i, a[a_abs]);
        if(b_abs >= 0 && b_abs < b_len)
            digit_add(acc+i, b[b_abs]);
    }

    return acc;
}

char* chengfa(char* a, char* b){
    int i, j;
    char* acc;
    char fc = 0;
    int a_p, b_p, acc_p;    
    int a_len = number_len(a), b_len = number_len(b);
    int acc_len;

    a_p = find_point(a);
    b_p = find_point(b);
    acc_p = a_p+b_p;

    acc_len = a_len+b_len-(a_len != a_p)-(b_len != b_p)+1; // 包括点号的长度
    acc = initLongNumerical(acc_len);
    acc[acc_p] = POINT;

    for (i = 0; i < a_len; i++){
        if(i == a_p)  continue;
        for(j = 0; j < b_len; ++j){
            if(j == b_p) continue;
            // 处理浮点位置
            int pos_re = i-a_p+(i-a_p < 0)+(j-b_p < 0)+j-b_p - ((i-a_p+(i-a_p < 0)+(j-b_p < 0)+j-b_p) <= 0);
            digit_add(acc+pos_re+acc_p, (int)a[i]*b[j]);
        }
    }

    return acc;
}

char* chu(char *a, char *b){
    char *b_t = reciprocal(b);
    char *acc = chengfa(a, b_t);
    
    free(b_t);
    return acc;
}

char* reciprocal(char* a){
    char *acc;
    int a_len = number_len(a);
    int a_p = find_point(a);
    int a_e = first_efective(a);
    int a_re = a_p - a_e;
    int x = CAP / a[a_e];
    int acc_p = (a_re < 0 ? -a_re : 0); // point 位置

    acc = initLongNumerical(abs(a_re)+1); // point 位置
    acc[acc_p] = POINT;
    acc[acc_p+a_re] = x;
    // acc right
    // 精度
    int n=1; // 某种程度上充当有效位置
    int endt;
    int Zerot;
    char TWO[3] = {'2'-'0', END, 0};

    int loop = 1;
    while(loop){
        n = 2*n;
        if (n > (a_len-a_e)){

            loop = 0;
        }

        if(n <= a_len-a_e){
            endt = a[a_e+n]; // 存储数据
            Zerot = a[a_e+n+1];
            a[a_e+n] = END;
            a[a_e+n+1] = '\0';
        }
        char* t1 = chengfa(a, acc); // x(2 - x * a[...])
        take_negative(t1);
        char *sum = add(TWO, t1);
        char* t2 = chengfa(acc, sum);

        free(acc);
        free(t1);
        free(sum);

        acc = t2;
        // 恢复数据
        if(n <= a_len-a_e){
            a[a_e+n] = endt;
            a[a_e+n+1] = Zerot;
        }
    }
    acc = accuracy_decoration(acc, n);
    return acc;
}

/* 辅助函数 */
int find_point(char *a){
    int i = 0;
    i = 0;
    while(isPoint(a[i]) && a[i] != END){
        ++i;
    }
    return i;
}

char isPoint(char ch){
    return ch ^ POINT;
}

int number_len(char* a){
    int i=0; // 含point
    while(a[i] != END) ++i;

    return i;
}

void digit_add(char* acc, int increase){
    int fc;
    while(increase != 0){
        if(*acc == POINT)
            ;
        else{
            fc = (*acc + increase) / CAP;
            *acc = (*acc + increase) % CAP;
            increase = fc;
        }
        --acc;
    }
}

int first_efective(char* a){
    int i;
    i = 0;
    while(a[i] == 0 || a[i] == END || a[i] == POINT) ++i;
    return i;
}

void take_negative(char *a){
    while(*a != END){
        if(*a == POINT) ; //pass
        else *a = -(*a);

        ++a;
    }
}

void show(char *a){
    int i;
    for(i=0; a[i] != END; ++i)
        printf("%d ", a[i]);
    printf("\n");
}

char* accuracy_decoration(char *a, int accuracy){
    int i;
    int a_e = first_efective(a);
    int a_l = number_len(a); // 其实可以一步完成信息获取, 获取需要封装for循环
    int a_p = find_point(a);

    a_e = __min(a_e, a_p);
    // 危险操作? 可能有潜在危险, 在多线程操作下, 危险更为突出
    // free(a);
    // a = (char *)realloc();

    char *acc;
    int acc_l = __min(a_l - a_e, accuracy);
    acc = initLongNumerical(acc_l);

    strcpy(acc, a+a_e);
    acc[acc_l] = END;
    acc[acc_l+1] = '\0';

    free(a);
    return acc;
}

void refresh(char *number){
    // 清零
    while(*number == 0) number += 1;

    int i;
    int sign = (*number >= 0) * 2 - 1;
    int len = number_len(number);
    for(i=len-1; i >= 0; --i){
        if(number > 0 && sign < 0){
            digit_add(number, sign*CAP);
            digit_add(number-1, -sign);
        }
        else if(number < 0 && sign > 0){
            digit_add(number, sign*CAP);
            digit_add(number-1, -sign);
        }
    }
}

char * initLongNumerical(int len){
    char *acc;
    acc = (char *)calloc(sizeof(char), (len+2));
    acc[len] = END;
    acc[len+1] = '\0';
    return acc;
}

char* atochar(char* ch){
    int i;
    int p;
    int acc_p;
    int Sign = 1;
    char *acc;

    if(*ch == '-'){
        Sign = -1;
        ch += 1;
    } // 获得符号

    // 获取其他信息
    for(i=0; ch[i] != '.' && ch[i] != '\0'; ++i) ;
    // printf("%s\n", ch);
    p = i;
    acc_p = (p+1) >> 1;
    acc = initLongNumerical(((strlen(ch)+1) >> 1) + 1);
    acc[acc_p] = POINT;

    if(p != 0){
        for(i=p-1; i >= 0; i -= 2){
            acc[i >> 1] = ch[i] - '0';
            if(i-1 >= 0) acc[i >> 1] += (ch[i-1] - '0') * 10;
            acc[i>>1] *= Sign;
        } // 整数部分
    }

    if(ch[p] != '\0'){
        for(i=p+1; ch[i] != '\0'; i += 2){
            acc[(i>>1)+1] = (ch[i] - '0')*10;

            if(ch[i+1] != '\0'){
                acc[(i>>1)+1] += (ch[i+1] - '0');
            }else {
                acc[(i>>1)+1] *= Sign;
                break;
            }
            acc[(i>>1)+1] *= Sign;
        }
    }

    return acc;
}

char* chartoa(char *chr){
    int i;
    char ch[4];
    char* str;
    refresh(chr);

    while(*chr == 0) chr += 1;
    str = (char*)calloc(strlen(chr)*2+2, sizeof(char));
    
    if(*chr < 0) strcat(str, "-"); // 标识正负
    for(i=0; chr[i] != END; ++i){
        if(chr[i] == POINT) {
            strcat(str, ".");
            continue;
        }
        my_itoa_10(abs(chr[i]), ch, 2);
        strcat(str, ch);
    }
    return str;
}

void my_itoa_10(int num, char* ch, int len){ // 十进制转换
    int i;
    int radix = 10;

    ch[len] = '\0';
    for(i=len-1; i >=0; --i){
        ch[i] = num % radix + '0';
        num /= radix;
    }
}

#endif
