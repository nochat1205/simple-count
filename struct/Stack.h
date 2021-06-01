// - - - - - 栈的顺序存储 - - - - - -
// - - - - - 先进先出 - - - - - - - 
#ifndef MY_STACK_H
#define MY_STACK_H
#include <stdlib.h>
#define STACK_INIT_SIZE 100
#define STACKINCREMENT 10
// #define DEBUG
// SELemType 存储类型
#ifdef DEBUG
#define SElemType int
#endif

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define EMPTY 0
typedef int Status;

typedef struct {
    SElemType *base;
    SElemType *top;
    int stacksize;
}SqStack;
// - - - - - 基本操作的函数原型说明 - - - -
    // 构造空栈 
Status InitStack(SqStack *S);
    // 摧毁栈s,s不再存在
Status DestroyStack(SqStack *S);
    // 把s置为空栈
Status ClearStack(SqStack *S);
    // 判断是否为空栈
Status StackEmpty(SqStack *S);
    // 判断是否栈满
Status StackFull(SqStack *S);
    // 返回S的元素个数,即栈的长度
int StackLength(SqStack *S);
    // 若栈不空,则用e返回S的栈顶元素,并返回OK;否则返回ERROR
Status GetTop(SqStack *S, SElemType *e);
    // 插入元素e为新的栈顶元素
Status Push(SqStack *S, SElemType *e);
    // 若栈不空,则删除S的栈顶元素,用e返回真值,并返回OK;否则ERROR
Status Pop(SqStack *S, SElemType *e);
    // 从栈底到栈顶依次对栈中每个元素调用函数visit().
Status StackTraverse(SqStack *S, Status (*visit)(SElemType *));
    // 一旦visit()失败,则操作失败

/* - - - - - - detail fun - - - - - - */
Status InitStack(SqStack *S) {
    S->base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if(!S->base) return ERROR;
    S->top = S->base;
    S->stacksize = STACK_INIT_SIZE;
    return OK;
}
Status DestroyStack(SqStack *S) {
    free(S->base);
    free(S);
    return OK;
}
Status ClearStack(SqStack *S) {
    S->top = S->base;
    return OK;
}
    // 判断是否为空栈
Status StackEmpty(SqStack *S) {
    if(S->top == S->base) return TRUE;
    return FALSE;
}
    // 返回S的元素个数,即栈的长度
int StackLength(SqStack *S) {
    return S->top - S->base;
}
    // 若栈不空,则用e返回S的栈顶元素,并返回OK;否则返回ERROR
Status GetTop(SqStack *S, SElemType *e) {
    if(S->top == S->base) return ERROR;
    *e = *(S->top - 1);
    return OK;
}

// 插入元素e为新的栈顶元素
Status Push(SqStack *S, SElemType *e) {
    if((S->top - S->base) >= S->stacksize) {
        S->base = (SElemType *)realloc(S->base,
                 (S->stacksize + STACKINCREMENT) * sizeof(SElemType));
        if(!S->base) return ERROR;
        S->top = S->base + S->stacksize;
        S->stacksize += STACKINCREMENT;
    }
    *(S->top) = *e;
    ++(S->top);
    return OK;
}
    // 若栈不空,则删除S的栈顶元素,用e返回真值,并返回OK;否则ERROR
Status Pop(SqStack *S, SElemType *e) {
    if(S->top == S->base) return ERROR;
    *e = *(S->top - 1);
    --(S->top);
    return OK;
}
    // 从栈底到栈顶依次对栈中每个元素调用函数visit().
    // 一旦visit()失败,则操作失败
Status StackTraverse(SqStack *S, Status (*visit)(SElemType *)) {
    while(S->base != S->top) (*visit)(S->base++);
    if(S->base == S->top) return OK;
    return ERROR;
}

Status StackFull(SqStack *S) {
    if(StackLength(S) == S->stacksize) return TRUE;
    return FALSE;
}

#endif
