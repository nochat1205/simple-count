#include <stdlib.h>
#include "mylib.h"
#include "unix_list.h"
#include "string_list.h"
/* 是否允许覆盖?  是
 * 每个变量内部独立
 */

typedef struct trie_tree{
    char name;
    /* trie 结构基础 */
    my_list_head sibling;           // 作为子链的标识,被上级children所链接
    my_list_head children;          // 可以改成二叉平衡树 增加检索速度, 只为检索

    /* 下列部分部分应该独立出来 data部分 */
    int quantity;                   // 参数数量 标识常量或者函数, 或者-1 符号, 用来减少
    char* value;                    // 计算后的值 
    my_list_head data_head;         // 函数固定时固定时固定, 判断是否使用
                                    // format 风格参数数量 二级指针, 便于检索, 如果是链表 可能无法定位
    my_list_head parameters_head;   // 参数列表
    my_list_head derivative_head;   // 衍生
}trie;


// 主要函数
void trie_init__(trie** node_ptr, char name, int quantity, my_list_head* sibling_prev, my_list_head* sibling_next);
trie* trie_add_name(trie* root, char name);
trie* trie_add_names(trie* root, char *names);
trie* trie_search_name(trie* root, char name);
trie* trie_search_names(trie* root, char* names);
void trie_count(trie* root, my_list_head* Operation);
int isParameterFor(trie *node, char* name);
int isFunction(trie* node);
int isSymbol(trie* node);
void trie_del(trie* root);
// 展示
void print_data(trie* root, char *str);

// 增 删 改 查
// 增
void trie_init__(trie** node_ptr
                , char name
                , int quantity
                // , my_list_head* data_opev
                // , my_list_head* data_next
                , my_list_head* sibling_prev
                , my_list_head* sibling_next
                // , my_list_head children
                ){
    // 最基本的初始化
    *node_ptr = (trie *)malloc(sizeof(struct trie_tree));
    trie * node = *node_ptr;
    node->name = name;
    node->quantity = quantity;
    node->value = NULL;
    MY_INIT_LIST_HEAD(&(node->data_head));
    MY_INIT_LIST_HEAD(&(node->parameters_head));
    MY_INIT_LIST_HEAD(&(node->derivative_head));

    // init sibling
    MY_INIT_LIST_HEAD(&(node->sibling));
    if(sibling_prev != NULL)
        my_list_add_next(sibling_prev, &(node->sibling));
    else if(sibling_next != NULL)
        my_list_add_prev(sibling_next, &(node->sibling));
    MY_INIT_LIST_HEAD(&(node->children));
}

trie* trie_add_names(trie* root, char *names){
    trie *children;
    my_list_head* pos;
    while(*names != '\0'){
        root = trie_add_name(root, *names);
        ++names;
    }
    return root;
}

trie* trie_add_name(trie* root, char name){
    trie* pos;
    trie* new_node; // 申请空间后, 由trie管理
    my_list_for_each_entry(pos, &(root->children), sibling, struct trie_tree){
        if(pos->name == name) return pos;
        if(name < (pos->name)) break;
    }
    trie_init__(&new_node, name, 0, NULL, &(pos->sibling));
    return new_node;
}

/* n个 具体赋值函数, 需要借助arg... */

/* 查 */
trie* trie_search_name(trie* root, char name){
    trie* ptr;
    my_list_for_each_entry(ptr, &(root->children), sibling, trie){
        if(ptr->name == name)
            return ptr;
    }
    return NULL;
}

trie* trie_search_names(trie* root, char* names){
    int i;
    char ch;
    trie* ptr;
    int flag = 0;
    for(i=0; (ch = names[i]) != '\0'; ++i){
        flag = 0;
        my_list_for_each_entry(ptr, &(root->children), sibling, trie){
            if(ptr->name == ch){
                root = ptr;
                flag = 1;
                break;
            }
        }
    }
    if(flag == 1 && !my_list_empty(&(root->data_head))) return root;
    else return NULL;
}

/* 删 */
void trie_del(trie* root){
    trie* ptr, *n;
    my_list_for_each_entry_safe(ptr, n, &(root->children), sibling, trie){
        free(ptr);
    }
    free(root);
}

// 展示
void print_data(trie* root, char *str){
    trie *node = trie_search_names(root, str);
    printf("%s = ", str);
    string_print_str(&(root->data_head));
}

void print_value(trie* root, char *str){
    trie *node = trie_search_names(root, str);
    printf("%s = %s ", str, root->value);
}

void trie_set_data(trie* root, trie* new_node, char* v_name, char *str[], int len){
    int i;
    for (i = 0; i < len; i++){
        trie* base_node;
        if(isNumeric(str[i])) continue;
        base_node = trie_search_names(root, str[i]);
        if(base_node == NULL) continue;
        if(isSymbol(base_node) || isFunction(base_node)) continue;
        if(isParameterFor(new_node, str[i]) >= 0) continue;
        if(string_search_name(&(base_node->derivative_head), v_name) == NULL){
            string_list_push_elem(&(base_node->derivative_head), v_name);
        }
    }
    string_list_push_expression(&(new_node->data_head), str, len);
}

void trie_set_parameters(trie* new_node, char *str[], int quantity){ // 参数字符串 参数个数
    new_node->quantity = quantity;
    string_list_push(&(new_node->parameters_head), str, quantity, NULL, NULL);
}

int isFunction(trie* node){
    return node->quantity > 0;
}

int isSymbol(trie* node){
    return node->quantity < 0;
}

int isParameterFor(trie *node, char* name){
    int i = 0;
    string_list* ptr;
    my_list_for_each_entry(ptr, &(node->parameters_head), sibling, string_list){
        if(strcmp(ptr->str, name) == 0) return i;
        i += 1;
    }
    return -1;
}

void trie_count(trie* root, my_list_head* Operation){ // count 主函数
    string_list *pos;

    // string_print_str(Operation);
    my_list_for_each_entry(pos, Operation, sibling, string_list){
        trie* data_node;
        if(isNumeric(pos->str)) {
            count_elem(pos->str);
            continue;
        }

        data_node = trie_search_names(root, pos->str);          // 不检查
        if(data_node->value != NULL){
            count_elem(data_node->value);
            continue;
        }
        else if(isSymbol(data_node)) {
            count_elem(pos->str);
            continue;
        }
        else if(isFunction(data_node)){
            int i;
            string_list *func;
            my_list_head func_expression;
            char* names[(data_node->quantity)];
            MY_INIT_LIST_HEAD(&func_expression);

            // 载入参数实际值
            for(i=0; i < (data_node->quantity); ++i){
                my_list_entry_next(pos, string_list, sibling); // for 循环前进
                names[i] = pos->str;
            }

            string_list *def_pos;
            string_list_push_elem(&func_expression, "(\0");
            my_list_for_each_entry(def_pos, &(data_node->data_head), sibling, string_list){
                int param_pos = isParameterFor(data_node, def_pos->str);
                if(param_pos >= 0) string_list_push_elem(&func_expression, names[param_pos]);
                else string_list_push_elem(&func_expression, def_pos->str);
            }
            string_list_push_elem(&func_expression, ")\0");

            trie_count(root, &func_expression);
        } // func 分析
        else {
            trie_count(root, &(data_node->data_head));
            char *t;
            GetTop(&opnd, &t);
            data_node->value = (char*)malloc(sizeof(char)*(strlen(t)+1));
            strcpy(data_node->value, t);
            printf("%s = %s\n", pos->str, data_node->value);
        }

    } // trie for

    return ;
}

void variable_clear_value(trie* root, trie* new_node){
    if(new_node->value == NULL) return;
    free(new_node->value);
    new_node->value = NULL;
    string_list* pos;
    my_list_for_each_entry(pos, &(new_node->derivative_head), sibling, string_list){
        trie* node;
        node = trie_search_names(root, pos->str);
        variable_clear_value(root, node);
    }
}

void trie_clear_base(trie* root, trie* new_node, char* v_name){
    // 清理表达式中的变量
    int i;
    string_list* pos, *n;
    my_list_for_each_entry_safe(pos, n, &(new_node->data_head), sibling, string_list){
        char* str = pos->str;
        trie* base_node;
        if(isNumeric(str)) ;
        else if((base_node = trie_search_names(root, str)) == NULL) return;
        else if(isSymbol(base_node) || isFunction(base_node)) ;
        else
            string_list_del_elem(&(base_node->derivative_head), v_name);
        string_list_del(pos);
    }
}
