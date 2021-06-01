/* 能写函数, 尽量写函数, 唯一操作一个函数
 * 函数-参数唯一性
 * 第一步 有用到才定义
 */
#ifndef MY_UNIX_LIST_H
#define MY_UNIX_LIST_H
#include <stdio.h>
typedef struct my_list_head{
    struct my_list_head *prev, *next;
}my_list_head;
#define my_offset_of(type, member) ((size_t)&(((type *)0)->member))

#define my_container_of(ptr, type, member) \
    (type *)((char *)ptr - my_offset_of(type, member))

#define MY_INIT_LIST_HEAD(ptr) do{\
    (ptr)->next = (ptr); \
    (ptr)->prev = (ptr);\
}while(0) // 将ptr 头节点化, 切断与前后节点的联系

#define MY_LIST_HEAD_INIT(name) {&(name), &(name)} // 根据已存在的name(不存在实体/空间), 形成一个节点, 

#define MY_LIST_HEAD(name) \
    struct list_head name = MY_LIST_HEAD_INIT(name)


#define my_list_for_each_next(pos, head) \
    for(pos = (head)->next; pos != (head); pos = pos->next)

#define my_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define my_list_entry_next(pos, type, member) \
    pos = my_container_of((pos->member).next, type, member)

#define my_list_for_each_entry(pos, head, member, type)                \
    for (pos = my_container_of((head)->next, type, member);    \
         &(pos->member) != (head);    \
         my_list_entry_next(pos, type, member))

// n指向下一个
#define my_list_for_each_entry_safe(pos, n, head, member, type)            \
    for (pos = my_container_of((head)->next, type, member),    \
         n = my_container_of(pos->member.next, type, member);    \
         &pos->member != (head);                     \
         pos = n, n = my_container_of(n->member.next, type, member))
/* 添加节点 */
void my_list_add( my_list_head *new_node, my_list_head *prev, my_list_head *next);
/* 向后添加节点 */
void my_list_add_next(my_list_head *prev, my_list_head *new_node);
void my_list_add_prev(my_list_head *next, my_list_head *new_node);
void my_list_connect(struct my_list_head *prev, struct my_list_head *next);
void my_list_del(struct my_list_head *entry);
int my_list_empty(struct my_list_head *head);

void my_list_add( my_list_head *new_node
                , my_list_head *prev
                , my_list_head *next) {
    next->prev = new_node;
    new_node->next = next;

    prev->next = new_node;
    new_node->prev = prev;
}

void my_list_add_next(my_list_head *prev
                    , my_list_head *new_node){
    my_list_add(new_node, prev, prev->next);
}

void my_list_add_prev(
          my_list_head *next
        , my_list_head *new_node ){
    my_list_add(new_node, next->prev, next);
}

void my_list_connect(
          struct my_list_head *prev
        , struct my_list_head *next){
    prev->next = next;
    next->prev = prev;
}

void my_list_del(struct my_list_head *entry){
    my_list_connect(entry->prev, entry->next);
    MY_INIT_LIST_HEAD(entry);
}

int my_list_empty(struct my_list_head *head){
    return head->next == head;
}

#endif
