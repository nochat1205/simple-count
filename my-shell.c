/* my_计算解释器 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Operation.h"
#include "struct/round_queue.h"
#include "struct/trie_tree.h"
#include "struct/mylib.h"

#define CAP 100
#define MAX_LINE        80 /* 80 chars per line, per command */
void my_exec(char *argv[], int argvlen);
int command_splite(char * buf, char *args[MAX_LINE/2 + 1]);
int isMyFunction(const char *, char * argv[MAX_LINE/2 + 1]);
int isHistory(char *buf);

struct round_queue* queue; // 常量声明
/* 全局变量 */
trie* root;
char *init[] = {"(\0", "+\0", "-\0", "*\0", "/\0", ")\0", ",\0", "^\0", 0};
int main(void){
    char *args[MAX_LINE/2 + 1]; /* command line (of 80) has max of 40 arguments */
    char buf[MAX_LINE];
    char* str;
    int Field_n;
    int status;
    int fd;
    int i;
    /* init need to free*/
    init_count();
    queue_init(&queue, CAP); // init thr history queue
    trie_init__(&root, '_', -1, NULL, NULL);
    i = 0;
    for(i = 0; (str = init[i]) != NULL; ++i){
        trie* new_node;
        new_node = trie_add_names(root, str);
        new_node->quantity = -1;   // 简单处理, 后续封装
        new_node->value = str;

        string_list_push(&(new_node->data_head), &str, 1, NULL, NULL);
    }

    /* main while */
    printf("osh>");
    while (fgets(buf, MAX_LINE, stdin) != NULL){ // 读取回车符
        i = 0;
        // queue_push(queue, buf);
        if(isHistory(buf)) { // history module
            printf("osh>");
            fflush(stdout);
            continue;
        }
        queue_push(queue, buf);

        /* 分割指令 */
        Field_n = command_splite(buf, args);
        /* 解析指令 */
        /* main */
        my_exec(args, Field_n);

        printf("osh>");
        fflush(stdout);

    } // while fget

    /* free */
    queue_free(queue);
    return 0;
}

int command_splite(char *buf, char *args[MAX_LINE/2 + 1] ) {
    int line = 0;
    int word = 0;
    // printf("line: %s", buf);

    while(*buf != '\n'){
        if(word == 1 && *buf == ' '){
            word = 0;
            *buf = '\0';
        }
        else if(word != 1 && *buf != ' '){ /* 字符 */
            word = 1;
            args[line] = buf;
            ++line;
        }
		++buf;
	}

    *buf = '\0';
    args[line] = NULL;
    return line;
}

int isHistory(char *buf){
    const int isReturn = 0;
    if(strcmp(buf, "\n") == 0){
        return 1;
    }
    if(strcmp(buf, "history\n") == 0) {
        queue_show(queue);
        return 1;
    }

    if(*buf == '!'){
        char *src;
        int position;
        if(*(buf+1) == '!') position = 1;
        else position = *(buf+1) - '0';
        if((src = queue_value_with(queue, position)) == NULL){
            if(position == 1)
                printf("No commads in history.\n");
            else
                printf("No such commad in history.\n");
            return !isReturn;
        }
        strcpy(buf, src);
        return isReturn;
    }
    return 0;
}

int isMyFunction(const char *args, char * argv[MAX_LINE/2 + 1]){
    int isReturn = 0;
    return isReturn;
}

void my_exec(char *argv[], int argvlen){
    /* 目前只有两种语句 
     * count 发给Operation 计算
     * print 输出变量情况
     * import 
     * 含有=号的 赋值语句 发给trie_tree 存储
     *  a = 赋值语句
     * func a b c x = a*x*x + b*x + c 函数定义语句
     */
    int i,j;
    int equal_pos;              // 标记等号位置
    int left_len;              // 左侧参数个数
    int right_len;              // 右侧参数个数
    int number_of_parameters;   // 标记参数个数

/* 非定义语句 */
    if(strcmp(argv[0], "count") == 0){
        char *ans;
        char *t;
        trie* node = trie_search_names(root, argv[1]);
        trie_count(root, &(node->data_head));
        Pop(&opnd, &t);
        node->value = (char*)malloc(sizeof(char)*(strlen(t)+1));
        strcpy(node->value, t);
        printf("%s = %s \n", argv[1], node->value);
        return ;
    }
    else if(strcmp(argv[0], "import") == 0){
        return ;
    }
    else if(strcmp(argv[0], "print") == 0){     // print 语句 完成
        trie* ptr;
        ptr = trie_search_names(root, argv[1]);
        printf("%s = ", argv[1]);
        string_print_str(&(ptr->data_head));
        return ;
    }

/* 定义语句 */
    /* 拆解语句 */

    i = 0;
    while(argv[++i] != NULL){
        if(strcmp(argv[i], "=") == 0)
            break;
    }

    equal_pos = i;
    number_of_parameters = equal_pos-1;
    right_len = argvlen - equal_pos - 1;    // 右侧参数个数
    left_len = equal_pos - 1;               // 减去变量名1
    /* 检查 */

/* 本质上, 都是函数, 区别在于固定参数, 不固定参数 */
    trie* new_node;
    new_node = trie_search_names(root, argv[0]);
    if(new_node == NULL)
        new_node = trie_add_names(root, argv[0]);   // 添加变量
    else{
        // 函数不重新定义
        trie_clear_base(root, new_node, argv[0]);
        variable_clear_value(root, new_node);
    }
/* 左值 */
    trie_set_parameters(new_node, argv+1, left_len);    // 初始化
/* 右值 */
    trie_set_data(root, new_node, argv[0], argv+equal_pos+1, right_len);
}
