#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct stat statbuf;

#define PRINT_STAT(action) \
    do { \
        stat("common_1.txt", &statbuf); \
        fprintf(stdout, action ": inode number = %ld, size = %ld bytes, blksize = %ld\n", \
                statbuf.st_ino, statbuf.st_size, statbuf.st_blksize); \
    } while (0)

int main()
{
    int fd1 = open("common_1.txt", O_CREAT | O_WRONLY);
    PRINT_STAT("open fd1");
    int fd2 = open("common_1.txt", O_CREAT | O_WRONLY);
    PRINT_STAT("open fd2");
    for (char c = 'a'; c <= 'z'; c++)
    {
        c % 2 ? write(fd1, &c, 1) : write(fd2, &c, 1);
        PRINT_STAT("write");
    }
    close(fd1);
    PRINT_STAT("close fd1");
    close(fd2);
    PRINT_STAT("close fd2");
    return 0;
}
