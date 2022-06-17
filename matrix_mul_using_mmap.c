#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 100

void get_row_and_col(char* mem, int* rc);	// 메모리 영역에 저장된 내용 바탕으로 행, 열 계산
int** mem_to_arr(char* mem, int row, int col);	// 메모리 영역에 저장된 내용을  정수 배열로 변환
void matrix_multiplication(int** matrix1, int* rc1, int** matrix2, int* rc2, int** result); // 행렬곱 계산
void print_matrix(int** matrix, int row, int col);	// 행렬 출력
void error_handling(char* message);

int main(int argc, char** argv) {
    int fd;
    char *file = NULL;
    int file_size;
	int i = 0;

    struct stat sb;
    char buf[BUF_SIZE];
    int flag = PROT_WRITE | PROT_READ;

	double start, end;

	// 각 행렬 행, 열 저장
    int* rc[2];
	for(i = 0; i < 2; i++)
		rc[i] = (int*)malloc(sizeof(int) * 2);

	// 각 행렬 저장
    int** matrix[2];

    if (argc < 3) 
	    printf("Usage: %s <file1 name> <file2 name>\n", argv[0]);

	// 실행 시간 측정 시작점
	start = (double)clock() / CLOCKS_PER_SEC;

	for (i = 0; i < 2; i++) {
		// 파일 열기
		if ((fd = open(argv[i + 1], O_RDWR|O_CREAT)) < 0)
			error_handling("file open error");

		// 파일 크기 정보 얻기 위해 fstat 함수 사용
		if (fstat(fd, &sb) < 0)
			error_handling("fstat error");
		file_size = (int)sb.st_size;

		// 파일 크기 바탕으로 동적할당
		file = (char *)malloc(file_size);

		// mmap을 이용한 메모리 매핑
		if ((file = (char *)mmap(0, file_size, flag, MAP_SHARED, fd, 0)) == NULL)
			error_handling("mmap error");

		get_row_and_col(file, rc[i]);
		matrix[i] = mem_to_arr(file, rc[i][0], rc[i][1]);

		munmap(file, file_size);
		close(fd);
	}

/*
	print_matrix(matrix[0], rc[0][0], rc[0][1]);
	printf("\n");
	print_matrix(matrix[1], rc[1][0], rc[1][1]);
	printf("\n");
	printf("%d %d %d %d\n", rc[0][0], rc[0][1], rc[1][0], rc[1][1]);
*/

	if (rc[0][1] != rc[1][0])
		error_handling("matrix multiplication error");

	int** result = (int**)malloc(sizeof(int*) * rc[0][0]);
	for (i = 0; i < rc[0][0]; i++) {
		result[i] = (int*)malloc(sizeof(int) * rc[1][1]);
	}

	// 행렬곱
	matrix_multiplication(matrix[0], rc[0], matrix[1], rc[1], result);
//	print_matrix(result, rc[0][0], rc[1][1]);

	// 실행 시간 측정 종료점
	end = (double)clock() / CLOCKS_PER_SEC;

	printf("[%dx%d]행렬과 [%dx%d]행렬의 곱셈 실행 시간: %lf\n", rc[0][0], rc[0][1], rc[1][0], rc[1][1], end - start);


	for (i = 0; i < 2; i++)
		free(rc[i]);
	
	for (i = 0; i < rc[0][0]; i++)
		free(result[i]);

    return 0;
}

void get_row_and_col(char* mem, int* rc) {
	int row = 0, col = 0;
	int i = 0, j = 0;

	// 열 계산
	while (1) {
		if (mem[i] == ' ')	
			col++;
		if (mem[i] == '\n') {
			col++;
			break;
		}
		i++;
	}

	// 행 계산
	i = 0;
	while (mem[i]) {
		if (mem[i] == '\n')
			row++;
		i++;
	}

	// 인자로 받은 배열에 행, 열 저장
	rc[0] = row;
	rc[1] = col;
}

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

	return matrix;
}

void print_matrix(int** matrix, int row, int col) {
	int i = 0, j = 0;

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			printf("%3d ", matrix[i][j]);
		}
		printf("\n");
	}
}

void matrix_multiplication(int** matrix1, int* rc1, int** matrix2, int* rc2, int** result) {
	int i = 0, j = 0, k = 0;
	
	for (i = 0; i < rc1[0]; i++) {
		for (j = 0; j < rc2[1]; j++) {
			result[i][j] = 0;
			for (k = 0; k < rc1[1]; k++) {
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}

void error_handling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
