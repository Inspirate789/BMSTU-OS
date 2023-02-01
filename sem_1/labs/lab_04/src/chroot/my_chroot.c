#include "apue.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>

#define _GNU_SOURCE



void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        err_quit("%s: it is impossible to get the maximum descriptor number ", cmd);

    if ((pid = fork()) < 0)
        err_quit("%s: fork function call error", cmd);
    else if (pid != 0)
        exit(0);

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: cannot ignore SIGHUP signal", cmd);

    if (chdir("/") < 0)
        err_quit("%s: cannot be made the current working directory /", cmd);

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    openlog(cmd, LOG_NDELAY, LOG_DAEMON);
    chdir("/home/inspirate/Documents/lab_04/src/fakeroot");

    if (!chroot("/home/inspirate/Documents/lab_04/src/fakeroot"))
        syslog(LOG_NOTICE, "Root directory changed to /home/inspirate/Documents/lab_04/src/fakeroot");
    else
        syslog(LOG_ERR, "Cannot change root directory to /home/inspirate/Documents/lab_04/src/fakeroot");
    
    // openlog(cmd, LOG_CONS, LOG_DAEMON);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "invalid file descriptors %d %d %d", fd0, fd1, fd2);

        exit(1);
    }
}

int main(void)
{
    time_t cur_time;
    daemonize("chroot_daemon");
    
    while (1)
    {
        cur_time = time(NULL);
        syslog(LOG_NOTICE, "Time: %s", ctime(&cur_time));
        sleep(10);
    }

    return 0;
}






















// SIMPLE EXAMPLE
// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>

// int main(int argc, char *argv[])
// {
//     chdir("/");
//     chdir("/home/inspirate/Documents/lab_04/src/fakeroot");
//     int result = chroot("/home/inspirate/Documents/lab_04/src/fakeroot");

//     if(result == 0)
//         printf("Chroot Success.\n");

//     execlp("ls", "ls", NULL);

//     return 0;
// }
