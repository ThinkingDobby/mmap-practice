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

int** mem_to_arr(char* mem);
void error_handling(char* message);

int main(int argc, char** argv) {
    int fd;
    char *file = NULL;
    int file_size;

    struct stat sb;
    char buf[BUF_SIZE];
    int flag = PROT_WRITE | PROT_READ;

    int** matrix1;
    int** matrix2;

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

    // 메모리에 있는 데이터를 행렬 형태로 변환
    matrix1 = mem_to_arr(file);

    munmap(file, file_size);
    close(fd);

    return 0;
}

int** mem_to_arr(char* mem) {
	int** matrix;
	int row = 0, col = 0;

	int i = 0;
	
	while (1) {
		if (mem[i] == ' ')
			col++;
		if (mem[i] == '\n') {
			col++;
			break;
		}
		i++;
	}

	i = 0;
	while (mem[i]) {
		if (mem[i] == '\n')
			row++;
		i++;
	}

	i = 0;
	int j = 0;
	// 이 부분에서 배열을 동적할당 하도록 하고
	int arr[4] = { 0, };

	// 이 밖에 반복문 하나 더 둘러서 모든 줄 정수로 변환할 것
	// 지금은 한 줄만 정수 변환
	for (i = 0; mem[i] != '\n'; i++) {
		if (mem[i] == ' ') 
			j++;
		else 
			arr[j] = arr[j] * 10 + (mem[i] - 48);
	}

	i = 0;
	for (i = 0; i < col; i++)
		printf("%d\n", arr[i]);

	// 이런 식으로 두 개 행렬을 모두 이차원 배열에 저장하면, 각 배열의 행, 열수 바탕으로
	// 행렬 곱 계산하는 함수 만들어 구현
	// 이후 실행 결과 확인

	return matrix;
}


void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
