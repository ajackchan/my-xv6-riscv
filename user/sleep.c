#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#ifndef STDERR
#define STDERR 2
#endif

int main(int argc,char *argv[])
{   
    if(argc != 2){
        fprintf(STDERR,"usage: sleep <second>\n");
        exit(1);
    }

    //字符串转整数
    int time = atoi(argv[1]);

    // 检查 atoi 返回值是否为0，且参数不是"0"
    if(time == 0 && argv[1][0] != '0'){
        fprintf(STDERR,"Invalid number: %s\n",argv[1]);
        exit(1);
    }

    //调用 sleep 系统调用
    sleep(time);
    exit(0);
}