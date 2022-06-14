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

void get_row_and_col(char* mem, int* rc);
int** mem_to_arr(char* mem, int row, int col);
void error_handling(char* message);

int main(int argc, char** argv) {
    int fd;
    char *file = NULL;
    int file_size;

    struct stat sb;
    char buf[BUF_SIZE];
    int flag = PROT_WRITE | PROT_READ;

    int rc[2];

    int** matrix1;
    int** matrix2;

    if (argc < 2) 
	    printf("Usage: %s <file name>\n", argv[0]);

    if ((fd = open(argv[1], O_RDWR|O_CREAT)) < 0)
	    error_handling("file open error");

    if (fstat(fd, &sb) < 0)
	    error_handling("fstat error");
    file_size = (int)sb.st_size;


    file = (char *)malloc(file_size);

    if ((file = (char *)mmap(0, file_size, flag, MAP_SHARED, fd, 0)) == NULL)
	    error_handling("mmap error");

    get_row_and_col(file, rc);
    matrix1 = mem_to_arr(file, rc[0], rc[1]);

    munmap(file, file_size);
    close(fd);

    return 0;
}

// 메모리상의 내용 바탕으로 행, 열 수 계산
void get_row_and_col(char* mem, int* rc) {
	int row = 0, col = 0;
	int i = 0, j = 0;

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

	rc[0] = row;
	rc[1] = col;
}

// 메모리상의 내용을 정수 배열로 변환
int** mem_to_arr(char* mem, int row, int col) {
	int** matrix;

	int i = 0, j = 0;

	// 행렬 저장할 배열 동적할당
	matrix = (int**)malloc(sizeof(int*) * row);
	for (i = 0; i < row; i++) {
		matrix[i] = (int*)malloc(sizeof(int) * col);
	}
	// 행렬 저장할 배열 초기화
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			matrix[i][j] = 0;
		}
	}

	// 메모리상에 저장된 내용을 연산 위해 정수 형태 행렬로 변환
	i = 0;
	int r = 0, tmp = 0;
	while (1) {
		if (mem[i] == '\n') {
			r++;
			tmp = 0;
			if (r == row)
				break;
		} else {
			if (mem[i] == ' ') 
				tmp++;
			else 
				matrix[r][tmp] = matrix[r][tmp] * 10 + (mem[i] - 48);
		}
		
		i++;
	}

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			printf("%3d ", matrix[i][j]);
		}
		printf("\n");
	}

	return matrix;
}


void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
