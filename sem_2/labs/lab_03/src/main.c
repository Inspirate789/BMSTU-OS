#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define PATH_LEN 1000
#define BUF_SIZE 1024

void print_page(uint64_t address, uint64_t data, FILE *out)
{
    fprintf(out, "0x%-15lx : %-10lx %-10ld %-10ld %-10ld %-10ld\n", address,
            data & (((uint64_t)1 << 55) - 1), (data >> 55) & 1, (data >> 61) & 1, (data >> 62) & 1, (data >> 63) & 1);
}

int get_pagemap_info(const char *proc, FILE *out)
{
    fprintf(out, "     addr         pfn  soft-dirty    file/shared    swapped     present\n");

    char path[PATH_LEN];
    snprintf(path, PATH_LEN, "/proc/%s/maps", proc);
    FILE *maps = fopen(path, "r");

    snprintf(path, PATH_LEN, "/proc/%s/pagemap", proc);
    int pm_fd = open(path, O_RDONLY);

    char buf[BUF_SIZE + 1] = "\0";
    int len;

    int present = 0;

    // чтение maps
    while ((len = fread(buf, 1, BUF_SIZE, maps)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            if (buf[i] == 0)
            {
                buf[i] = '\n';
            }
        }
        buf[len] = '\0';

        // проход по строкам из maps
        char *save_row;
        char *row = strtok_r(buf, "\n", &save_row);

        while (row)
        {
            // получение столбца участка адресного пространства
            char *addresses = strtok(row, " ");
            char *start_str, *end_str;

            // получение начала и конца участка адресного пространства
            if ((start_str = strtok(addresses, "-")) && (end_str = strtok(NULL, "-")))
            {
                uint64_t start = strtoul(start_str, NULL, 16);
                uint64_t end = strtoul(end_str, NULL, 16);

                for (uint64_t i = start; i < end; i += sysconf(_SC_PAGE_SIZE))
                {
                    uint64_t offset;
                    // поиск смещения, по которому в pagemap находится информация о текущей странице
                    uint64_t index = i / sysconf(_SC_PAGE_SIZE) * sizeof(offset);

                    pread(pm_fd, &offset, sizeof(offset), index);
                    print_page(i, offset, out);
                    if (((offset >> 63) & 1) == 1)
                    {
                        present++;
                    }
                }
            }

            row = strtok_r(NULL, "\n", &save_row);
        }
    }

    fclose(maps);
    close(pm_fd);
    return present;
}

void get_maps_info(const char *proc, FILE *out)
{
    fprintf(out, "address                   perms  offset  dev   inode                 pathname\n");
    char path[PATH_LEN];
    snprintf(path, PATH_LEN, "/proc/%s/maps", proc);
    FILE *maps = fopen(path, "r");

    char buf[BUF_SIZE + 1] = "\0";
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, maps)) > 0)
    {
        for (int i = 0; i < len; i++)
            if (buf[i] == 0)
                buf[i] = '\n';
        buf[len] = '\0';
        if (buf[1] == '\n' && buf[0] == ' ')
            buf[1] = ' ';

        char *save_row;
        char *row = strtok_r(buf, "\n", &save_row);

        while (row)
        {
            fprintf(out, "%s\n", row);
            row = strtok_r(NULL, "\n", &save_row);
        }
    }

    fclose(maps);
}

void get_stat_info(const char *proc, FILE *out)
{
    char path[PATH_LEN];
    snprintf(path, PATH_LEN, "/proc/%s/stat", proc);
    FILE *stat = fopen(path, "r");

    char *headers[] = {"pid", "comm", "state", "ppid", "pgrp", "session", "tty_nr", "tpgid", "flags", "minflt", "cminflt", "majflt", "cmajflt", "utime", "stime", "cutime", "cstime", "priority", "nice", "num_threads", "itrealvalue", "starttime", "vsize", "rss", "rsslim", "startcode", "endcode", "startstack", "kstkesp", "kstkeip", "signal", "blocked", "sigignore", "sigcatch", "wchan", "nswap", "cnswap", "exit_signal", "processor", "rt_priority", "policy", "delayacct_blkio_ticks", "guest_time", "cguest_time", "start_data", "end_data", "start_brk", "arg_start", "arg_end", "env_start", "env_end", "exit_code"};

    char buf[BUF_SIZE + 1] = "\0";
    int len;

    while ((len = fread(buf, 1, BUF_SIZE, stat)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            if (buf[i] == 0)
            {
                buf[i] = '\n';
            }
        }
        buf[len] = '\0';

        char *save_row;
        char *row = strtok_r(buf, " ", &save_row);
        int i = 0;
        while (row)
        {
            fprintf(out, "%s  %s\n", headers[i], row);
            row = strtok_r(NULL, " ", &save_row);
            i++;
        }
    }

    fclose(stat);
}

void get_task_info(const char *proc, FILE *out)
{
    DIR *d;
    struct dirent *dir;
    char dir_path[PATH_LEN];
    snprintf(dir_path, PATH_LEN, "/proc/%s/task/", proc);
    d = opendir(dir_path);
    fprintf(out, "\nTHREADS\n");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            fprintf(out, "%s\n", dir->d_name);
        }
        closedir(d);
    }
}

int main(int argc, char *argv[])
{
    char *pid = argv[1];
    FILE *file = fopen("output.txt", "w");
    if (!file)
    {
        perror("fopen() error");
        exit(1);
    }
    // int present = get_pagemap_info(pid, file);
    get_maps_info(pid, file);
    get_stat_info(pid, file);
    get_task_info(pid, file);
    // printf("prsent = %d\n", present);
    return 0;
}
