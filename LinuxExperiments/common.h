//
// Created by andtokm on 04.06.22.
//

#ifndef CPPPROJECTS_COMMON_H
#define CPPPROJECTS_COMMON_H

/* ANSI headers to which POSIX adds extensions. */
#include <cassert>
#include <climits> /* NZERO */
#include <cmath> /* M_PI, M_PI_2, M_PI_4 */

#include <cstdio> /* popen(), perror() */
#include <cstdint>
#include <cstdlib>
#include <cstring> /* strerror, strdup */
#include <strings.h> /* ffs */

/* POSIX only headers. */
#include <arpa/inet.h>
// #include <curses.h>
#include <dirent.h>
#include <cerrno>
#include <fcntl.h> /* creat, O_CREAT */
#include <libgen.h>
#include <monetary.h> /* strfmon */
#include <netdb.h> /* gethostbyname */
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h> /* getpwuid, getpwnam, getpwent */
#include <regex.h>
#include <sched.h>
#include <sys/mman.h> /* mmap, munmap */
#include <sys/ipc.h>
#include <sys/resource.h> /* rusage, getrusage, rlimit, getrlimit */
#include <sys/select.h> /* select, FD_ZERO, FD_SET */
#include <sys/sem.h> /* semget, semop, semctl */
#include <sys/shm.h> /* shmget, shmat, etc. */
#include <sys/socket.h>
#include <sys/stat.h> /* S_IRUSR and family, */
#include <sys/types.h> /* pid_t */
#include <sys/time.h>
#include <sys/utsname.h> /* uname, struct utsname */
#include <sys/wait.h> /* wait, sleep */
#include <syslog.h> /* syslog */
#include <termios.h>
#include <unistd.h> /* read, fork, ftruncate */

#endif //CPPPROJECTS_COMMON_H
