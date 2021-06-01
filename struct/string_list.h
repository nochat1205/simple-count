#ifndef STRING_LIST_H
#define STRING_LIST_H

#include "unix_list.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
typedef struct string_list{
    char * str;
    my_list_head sibling;
}string_list;

// 增删改查
// 增
// 初始化并添加一项
string_list* string_search_name(my_list_head* list, char* name);
void string_node_init(struct string_list **my_self_ptr, char *str, my_list_head* sibling_prev, my_list_head* sibling_next);
// 添加单项
void string_list_push_elem(my_list_head* data_head_p, char *str);
// 添加多项 left / right 为前后项
void string_list_push(my_list_head* data_head_p, char *str[], int sentenlen, char *left, char* right);
// 拼接两个
void string_list_connact(my_list_head* list_head_p0, my_list_head* list_head_p1);
void string_list_push_expression(my_list_head* data_head_p, char *str[], int sentenlen);
// 删
void string_list_del(string_list* my_self);
void string_list_del_elem(my_list_head* self, char* v_name);
// 看
void string_get_str_s(my_list_head* data_head_p, char *buf, int len);
void string_print_str(my_list_head* data_head_p);
// 查找
string_list* string_search_name(my_list_head* list, char* name);

void string_node_init(struct string_list **my_self_ptr
    , char *str
    , my_list_head* sibling_prev
    , my_list_head* sibling_next ){
     // 使用指针, 将空间交给父级管控
    int len = strlen(str);
    *my_self_ptr = (string_list*)malloc(sizeof(struct string_list));

    string_list *my_self = *my_self_ptr;
    my_self->str = (char *)malloc((len+2)*sizeof(char));
    strcpy(my_self->str, str);

    MY_INIT_LIST_HEAD(&(my_self->sibling));
    if(sibling_prev != NULL){
        my_list_add_next(sibling_prev, &(my_self->sibling));
    }else if(sibling_next != NULL){
        my_list_add_prev(sibling_next, &(my_self->sibling));
    }
}

// 添加单项
void string_list_push_elem(my_list_head* data_head_p, char *str){
    my_list_head* next = data_head_p;       // &data_head != data_head->prev
    string_list* str_node;
    string_node_init(&str_node, str, NULL, next);
}

// 添加多项 left / right 为前后项
void string_list_push(my_list_head* data_head_p, char *str[], int sentenlen, char *left, char* right){
    int i;
    my_list_head* next = data_head_p;       // &data_head != data_head->prev
    string_list* str_node;

    if(left != NULL)
        string_node_init(&str_node, left, NULL, next);
    for(i=0; i < sentenlen; ++i)
        string_node_init(&str_node, str[i], NULL, next);
    if(right != NULL)
        string_node_init(&str_node, right, NULL, next);
}

// 拼接两个
void string_list_connact(my_list_head* list_head_p0, my_list_head* list_head_p1){
    string_list* ptr;
    my_list_for_each_entry(ptr, list_head_p1, sibling, string_list){
        string_list* str_node;
        string_node_init(&str_node, ptr->str, NULL, list_head_p0);
    }
}

void string_list_push_expression(my_list_head* data_head_p, char *str[], int sentenlen){
    string_list_push(data_head_p, str, sentenlen, "(", ")");
}

// 删
void string_list_del(string_list* my_self){
    my_list_del(&(my_self->sibling));
    free(my_self->str);
    free(my_self);
}

void string_list_del_elem(my_list_head* self, char* v_name){
    string_list* pos;
    pos = string_search_name(self, v_name);
    string_list_del(pos);
}

// 看
void string_get_str_s(my_list_head* data_head_p, char *buf, int len){
    string_list *ptr;
    int pos = 0;
    my_list_for_each_entry(ptr, data_head_p, sibling, struct string_list){
        int i = 0;
        char *str = ptr->str;
        while(len > pos && str[i] != '\0')
            buf[pos++] = str[i++];
        if(len == pos){
            buf[len-1] = '\0';
            return;
        }
        buf[pos++] = ' ';
    }
    buf[pos++] = '\0';
}

void string_print_str(my_list_head* data_head_p){
    string_list *ptr;
    my_list_for_each_entry(ptr, data_head_p, sibling, struct string_list){
        printf("%s ", ptr->str);
    }
    printf("\n");
}

string_list* string_search_name(my_list_head* list, char* name){
    string_list *pos;
    my_list_for_each_entry(pos, list, sibling, struct string_list){
        if(strcmp(pos->str, name) == 0) return pos;
    }
    return NULL;
}
#endif
