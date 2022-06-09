#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 100

void error_handling(char* message);

int main(int argc, char** argv) {
    int fd;
    char *file = NULL;
    int file_size;

    struct stat sb;
    char buf[BUF_SIZE];
    int flag = PROT_WRITE | PROT_READ;

    if (argc < 2) 
	    printf("Usage: %s <file name>\n", argv[1]);

    if ((fd = open(argv[1], O_RDWR|O_CREAT)) < 0)
	    error_handling("file open error");

    if (fstat(fd, &sb) < 0)
	    error_handling("fstat error");
    file_size = (int)sb.st_size;


    file = (char *)malloc(file_size);

    if ((file = (char *)mmap(0, file_size, flag, MAP_SHARED, fd, 0)) == NULL)
	    error_handling("mmap error");

    printf("%s", file);

    munmap(file, file_size);
    close(fd);

    return 0;
}

void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
