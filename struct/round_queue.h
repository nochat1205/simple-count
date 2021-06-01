
#ifndef ROUND_QUEUE_H
#define ROUND_QUEUE_H

#ifndef MAX_LINE
#define MAX_LINE        80 /* 80 chars per line, per command */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TYPEP char *
#define TYPE char

struct round_queue;
#define round_queue_for(Type) \
    struct round_queue{\
        Type *list;\
        int head;\
        int tail;\
        int mod;\
    }\

round_queue_for(TYPEP);
/* queue function */
void queue_init(struct round_queue**, int n);
void queue_push(struct round_queue*, char *str);
char *queue_value_with(struct round_queue*, int);
void queue_free(struct round_queue*);
void queue_show(struct round_queue*);

void queue_init(struct round_queue **queue_pos, int n){
    int i;
    // struct round_queue *queue = *queue_pos;
    // // 地址申请
    struct round_queue *queue = (struct round_queue*)malloc(sizeof(struct round_queue));
    if(!queue) exit(1);
    queue->mod = n;
    queue->head = n-1;
    queue->list = (TYPEP *)malloc(sizeof(TYPEP)*n);
    for(i=0; i < n; ++i) queue->list[i] = NULL;
    *queue_pos = queue;
}

void queue_push(struct round_queue *queue, char *str){
    if(queue->list[queue->head] == NULL)
        queue->list[queue->head] = (TYPEP)malloc(MAX_LINE*sizeof(TYPE));
    // printf("%d\n", queue->head);
    // printf("%d\n", queue->head);
    strcpy(queue->list[queue->head], str);
    queue->head = (queue->head+queue->mod-1) % queue->mod; // -1 mod 6 = -1
}

char *queue_value_with(struct round_queue *queue, int position){
    // printf("%d %d \n", position, queue->head);
    if(position > queue->mod){
        printf("Position overflow");
        return NULL;
    }
    return queue->list[((queue->head)+position) % queue->mod];
}

void queue_free(struct round_queue *queue){
    int i = 0;
    for(i = 0; i < queue->mod; ++i)
        if(queue->list[(queue->head+(++i)) % queue->mod])
            free(queue->list[(queue->head+i) % queue->mod]);
    free(queue->list);
    free(queue);
}

void queue_show(struct round_queue *queue){
    int i;
    for(i = 1; i < (queue->mod)+1; ++i){
        // printf("%2d %s", (queue->head+i) % queue->mod, queue->list[(queue->head+i) % queue->mod]);
        if(queue->list[(queue->head+i) % queue->mod])
            printf("%2d %s", i, queue->list[(queue->head+i) % queue->mod]);
        else break;
    }
}

#endif
