#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char* path, char* target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开路径
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return ;
    }

    // 获取文件/目录的状态
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);

        return ;
    }

    //判断文件类型
    switch (st.type) {
    case T_FILE:
        if(strcmp(path + strlen(path) - strlen(target), target) == 0) {
			printf("%s\n", path);
		}

        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: path too long\n");
            break;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0) {
                continue;
            }

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if(stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n",buf);
                continue;
            }

            //跳过 '.' 和 '..'
            if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                find(buf,target);
            }
        }

        break;
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}