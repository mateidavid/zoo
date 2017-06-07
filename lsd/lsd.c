/*
 * List directories using getdents() because ls, find and Python libraries
 * use readdir() which is slower (but uses getdents() underneath.
 *
 * Ref: http://unix.stackexchange.com/a/120078
 */

#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

static const char * usage_message =
    "Usage: lsd [-c] [<dir>]\n"
    "List contents of directory <dir> (or the current directory).\n"
    "Options:\n"
    "  -h : print this help\n"
    "  -c : print count of file entries only\n";

struct linux_dirent
{
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[1];
};

#define BUF_SIZE 1024*1024*5

int
main(int argc, char *argv[])
{
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;
    char char_d_type;
    //
    int count_files = 0;
    unsigned num_files = 0;
    int expect_options = 1;
    // parse args
    int argi = 1;
    while (argi < argc)
    {
        if (expect_options && strlen(argv[argi]) == 2 && !strncmp(argv[argi], "--", 2))
        {
            expect_options = 0;
        }
        else if (expect_options && strlen(argv[argi]) == 2 && !strncmp(argv[argi], "-h", 2))
        {
            printf("%s", usage_message);
            exit(EXIT_SUCCESS);
        }
        else if (expect_options && strlen(argv[argi]) == 2 && !strncmp(argv[argi], "-c", 2))
        {
            count_files = 1;
        }
        else
        {
            break;
        }
        ++argi;
    }
    if (argi + 1 < argc)
    {
        printf("%s", usage_message);
        exit(EXIT_FAILURE);
    }
    fd = open(argi < argc ? argv[argi] : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");
    for (;;)
    {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");
        if (nread == 0)
            break;
        for (bpos = 0; bpos < nread;)
        {
            d = (struct linux_dirent *)(buf + bpos);
            d_type = *(buf + bpos + d->d_reclen - 1);
            switch (d_type)
            {
            case DT_BLK:  char_d_type = 'b'; break;
            case DT_CHR:  char_d_type = 'c'; break;
            case DT_DIR:  char_d_type = 'd'; break;
            case DT_FIFO: char_d_type = 'p'; break;
            case DT_LNK:  char_d_type = 'l'; break;
            case DT_REG:  char_d_type = 'f'; break;
            case DT_SOCK: char_d_type = 's'; break;
            default:      char_d_type = '?'; break;
            }
            if (d->d_ino != 0)
            {
                if (!count_files)
                {
                    putchar(char_d_type);
                    putchar(' ');
                    puts((char *)d->d_name);
                }
                else
                {
                    ++num_files;
                }
            }
            bpos += d->d_reclen;
        }
    }
    if (count_files)
        printf("%u\n", num_files);
    exit(EXIT_SUCCESS);
}
