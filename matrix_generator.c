#include <stdio.h>
#include <stdlib.h>

// 행렬 생성
// 입력: 파일명, 행, 열
int main(int argc, char* argv[]) {
	char fname[1000];

	int row = 0, col = 0;
	int i = 0, j = 0;

	int now = 1;

	scanf("%s", fname);
	fname[sizeof(fname) - 1] = 0;

	FILE* fp = fopen(fname, "wb");

	scanf("%d", &row);
	scanf("%d", &col);

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (j != col - 1)
				fprintf(fp, "%d ", now);	
			else
				fprintf(fp, "%d", now);
			now++;
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return 0;
}
