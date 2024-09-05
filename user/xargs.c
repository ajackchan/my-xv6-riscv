#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUF_SIZE 2048
#define MAX_ARGS MAXARG

// 带参数列表，执行某个程序
void run(char* program, char** args) {
    if(fork() == 0) {
        exec(program, args);
        fprintf(2, "exec %s failed\n", program);
    
        exit(1);
    }
}

// 处理读入的参数并执行
void execute_command(char* program, char** argsbuf, char** pa) {
    *pa = 0;    // 参数列表末尾用 null 标识列表结束
    run(program, argsbuf);  // 执行命令
}

int main(int argc, char* argv[]) {
    char buf[BUF_SIZE]; // 读入时使用的内存池
    char *p = buf, *last_p = buf;   // 当前参数的结束、开始指针
    char *argsbuf[MAX_ARGS];    //全部参数列表，包含 argv 传进来的参数和 stdin 读入的参数
    char **args = argsbuf;  // 指向 argsbuf 中第一个从 stdin 读入的参数

    // 将 argv 提供的参数加入到最终的列表中
    for(int i = 1; i < argc; i++) {
        if(args - argsbuf >= MAX_ARGS - 1) {
            //fprintf(2, "too many arguments\n");
            exit(1);
        }

        *args++ = argv[i];
    }

    char **pa = args;
    int n;

    while((n = read(0, p, 1)) > 0) {
        if(*p == ' ' || *p == '\n') {
            *p = '\0';  // 将空格或换行替换为 '\0'，分割开各个参数
            *pa++ = last_p;

            if(pa - argsbuf >= MAX_ARGS - 1) {
                fprintf(2, "too many arguments\n");
                exit(1);
            }

            last_p = p + 1;

            if(*p == '\n') {
                execute_command(argv[1], argsbuf, pa);
                pa = args;
            }
        }

        p++;
        if(p - buf >= BUF_SIZE) {
            fprintf(2, "input line too long\n");
            exit(1);
        }
    }

    if(n < 0) {
        fprintf(2, "read error\n");
        exit(1);
    }

    if(pa != args) {    // 如果最后一行不是空行
        *p = '\0';
        *pa++ = last_p;
        execute_command(argv[1], argsbuf, pa);       
    }

    while(wait(0) != -1) {};    // 循环等待所有子进程完成
    exit(0);
}