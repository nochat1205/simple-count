// 基本运算器 基础
// utf-8
//  #  +  -  (  )  *  .  /
// 35 43 45 40 41 42 46 47
// 废弃++ --
// import 所有实际占据空间的中间变量(malloc)都将在函数内释放

#ifndef OPERATION_H
#define OPERATION_H
#define SElemType char*          // 修改data类型 次序不能改
#include "./struct/Stack.h" 
#include <stdio.h>
#include <stdarg.h>
#include "./struct/unix_list.h"
#include "./struct/mylib.h"
#include "./struct/string_list.h"
#include "./struct/my_ALU.h"

#define BUFFSIZE 75

int Precede(char* t, char* k);
char* Operate(char*, int operatenum, char *param[]);  // 计算
char* Rank(int);  // 阶乘
char* my_pow(char*, char);      // 快速幂

SqStack optr;
SqStack opnd;
int Operatenum[128];
int Operation_b[128], Operation_a[128];

void init_count(){
        // 初始化 运算栈
    InitStack(&optr); // 初始化运算符栈, 与数字栈
    InitStack(&opnd);
    static char stack_bottom[] = {"#\0"};
    char* a = stack_bottom;
    Push(&optr, &a); // 预先设置真实栈底标志
    /* 运算元素数量 */
    Operatenum['!'] = 1;
    Operatenum['('] = 2;
    Operatenum['#'] = 2;
    Operatenum['+'] = 2;
    Operatenum['-'] = 2;
    Operatenum['*'] = 2;
    Operatenum['/'] = 2;
    Operatenum['^'] = 2;
    Operatenum['.'] = 2;
    Operatenum['1'] = 1;

    /* 优先级声明 */
    // 两位置一等
    // 不满足 a*b = b*a 交换律
    // 入栈运算符

    Operation_b['#'] = 0;
    Operation_b['+'] = 5;
    Operation_b['-'] = 5;
    Operation_b['*'] = 11;
    Operation_b['/'] = 11;
    Operation_b['('] = 1; // 零等公民
    Operation_b[')'] = 1;
    Operation_b['!'] = 57;
    Operation_b['^'] = 53;
    Operation_b['.'] = 81;
    Operation_b['1'] = 51;

    // 待入栈运算符
    Operation_a['#'] = 0;
    Operation_a['+'] = 4;
    Operation_a['-'] = 4;
    Operation_a['*'] = 10;
    Operation_a['/'] = 10;
    Operation_a['('] = 1;
    Operation_a[')'] = 1;
    Operation_a['!'] = 58;
    Operation_a['^'] = 54;
    Operation_a['.'] = 82;
    Operation_a['1'] = 52; // 负号

    // end
}

char* count_elem(char* elem){
    // 处理基本表达式, 不返回, 得数沉底运算符栈
    int i, j;
    char *e;        // 操作符
    char *buf;      // 接收弹出的数, 中继, 申请空间 opnd 接管
    string_list *pos;

    while(1){
        char *str = elem;

        if(isNumeric(str)){                 // 处理数字
            buf = (char *)calloc(BUFFSIZE, sizeof(char));
            my_strcat(buf, str, BUFFSIZE);
            Push(&opnd, &buf);
            break;
        }
        // else
        /*  栈内优先级比较
         * > 0 : 继续入栈
         * = 0 : 相等出栈(与), #与#
         * < 0 : 提取一个运算符
         */
        GetTop(&optr, &e); // 取出顶端操作符
        int imp;                    // 优先级比较结果
        imp = Precede(e, str);
        if(imp > 0){
            Push(&optr, &str);
            break;
        }
        else if(imp == 0) {
            Pop(&optr, &e);
            break;
        }else{
            char* res;
            char* theta;
            char* number_buf[4];
            // 根据算符提取元素, 进行运算
            Pop(&optr, &theta);
            for(i = 0; i < Operatenum[*theta]; i++)
                Pop(&opnd, &(number_buf[i]));

            res = Operate(theta, Operatenum[*theta], number_buf);
            Push(&opnd, &res);

            for(i = 0; i < Operatenum[*theta]; i++) // 数字栈会被free, 操作符栈不会
                free(number_buf[i]);

            continue;
        }
        break;
    }

    GetTop(&opnd, &buf);
    return buf;
}

/* - - - - - 函数区 - - - - -*/
int Precede(char* t, char* k) {
    // printf("compare:%c %c", t, k);
    static int rank = 0;  
    int grade_b, grade_a;
    grade_b = rank + Operation_b[*t];

    // '('运算律
    if(*k == '(') rank += 100;
    grade_a = rank + Operation_a[*k];

    if((grade_a - grade_b) == 0) rank -= 100;
    return grade_a - grade_b;
}

char* Operate(char* theta, int operatenum, char *param[]) {
    int i;
    char* acc;  // 存储计算结果
    char th = *theta;
    char *pa[operatenum];
// 转换
    for(i=0; i < operatenum; ++i){
        pa[i] = atochar(param[i]);
    }
// 计算
    if(th == '+')                   
        acc = add(pa[1], pa[0]);

    else if(th == '/' && *(pa[0]) != 0)    
        acc = chu(pa[1], pa[0]);

    else if(th == '*')              
        acc = chengfa(pa[1], pa[0]);

    else if(th == '-') {
        take_negative(pa[0]);
        acc = add(pa[1], pa[0]);
    }
    // else if(th == '!') return Rank((int)b);
    else if(th == '^') {
        acc = my_pow(pa[1], *pa[0]);
    }
    else if(th == '1') {
        take_negative(pa[0]);
        acc = (char*)malloc((strlen(pa[0])+2)*sizeof(char));
        strcpy(acc, pa[0]);
    }
    else exit(0);
// 转换  
    for(i=0; i < operatenum; ++i)
        free(pa[i]);

    char *ans;
    ans = chartoa(acc);
    free(acc);

    return ans;
}

char* Rank(int i) {
    char *ch;
    return ch;
}

char* my_pow(char* a, char b){
    char value[] = {'\1', END, '\0'};
    char *acc, *y;
    acc = value;
    y = a;
    while(b != 0){
        if((b & 1) == 1){
            acc = chengfa(acc, y);
        }
        y = chengfa(y, y);
        b = b >> 1;
    }
    return acc;
}

#endif
