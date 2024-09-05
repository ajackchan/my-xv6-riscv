#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

enum PIPE_END {REC = 0,SND = 1};

//执行素数筛选
void sieve(int pleft[2]) {
    int p;

    // 读取当前管道中的第一个数，即素数
    if(read(pleft[REC], &p, sizeof(p)) == 0 || p == -1) {
        // 如果读取失败或遇到哨兵值-1，则退出
        close(pleft[REC]);
        exit(0);
    }

    printf("prime %d\n", p);

    int pright[2];
    pipe(pright);   // 创建用于传递给下一个筛子的管道

    if(fork() == 0) {
        // 子进程，处理下一个筛子阶段
        close(pright[SND]);
        close(pleft[REC]);

        sieve(pright);  // 递归调用 sieve 处理下一个筛子阶段
    }
    else {
        // 父进程，负责筛选和传递数据
        close(pright[REC]);

        int buf;
        while(read(pleft[REC], &buf, sizeof(buf)) > 0 && buf != -1) {
            if(buf % p != 0) {
                write(pright[SND], &buf, sizeof(buf));
            }
        }

        close(pleft[REC]);
        close(pright[SND]);

        wait(0);
    }

    exit(0);
}

int main(int argc, char* argv[]) {
    int input_pipe[2];
    pipe(input_pipe); // 创建主进程的输入管道

    if(fork() == 0) {
        // 子进程，执行筛选
        close(input_pipe[SND]);  // 关闭子进程不需要的写入端
        sieve(input_pipe);
    }
    else {
        // 主进程，生成数列并传递给子进程
        close(input_pipe[REC]);

        for(int i = 2;i <= 35;i++) {
            write(input_pipe[SND], &i, sizeof(i));
        }

        int sentinel = -1;
        write(input_pipe[SND], &sentinel, sizeof(sentinel)); // 写入哨兵值表示结束
        close(input_pipe[SND]);   // 关闭写入端

        wait(0);  // 等待子进程完成
    }

    exit(0);
}