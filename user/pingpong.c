#include "kernel/types.h"
#include "user/user.h"

//REC、SND 分别代表管道的读取端和写入端
enum PIPE_END {REC = 0, SND = 1};

int main(int argc, char* argv[]) {
    if(argc != 1) {
        fprintf(2,"usage: pingpong (no parameter)\n");
        exit(1);
    }

    int p1[2]; // 父到子
    int p2[2]; // 子到父
    pipe(p1);
    pipe(p2);

    int pid = fork();

    if(pid == 0) { // 子进程
        close(p1[SND]);
        close(p2[REC]);

        char buf;
        if(read(p1[REC], &buf, 1) > 0) {
            printf("%d: recevied ping\n", getpid());
        }

        write(p2[SND], "p", 1);

        close(p1[REC]);
        close(p2[SND]);

        exit(0);
    }
    else if(pid > 0) { // 父进程
        close(p1[REC]);
        close(p2[SND]);

        write(p1[SND], "p", 1);

        char buf;
        if(read(p2[REC], &buf, 1) > 0) {
            printf("%d: received pong\n",getpid());
        }

        close(p1[SND]);
        close(p2[REC]);
        
        wait(0);
        exit(0);
    }
    else {
        fprintf(2, "failed to fork\n");
        exit(1);
    }

    exit(0);
}