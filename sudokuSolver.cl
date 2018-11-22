
//typedef struct __attribute__((packed)) sudokuPallet {
typedef struct _sudokuPallet {
	char n[9][9];
} sudokuPallet;

typedef struct _result {
	char r[10];
} result;


__kernel 
void vectorAdd(__global result *output,
                __global sudokuPallet *sudokuOut,
				__global sudokuPallet *sudokuIn,
				__global char *last,
				__global int *lastPallet,
				 int y, int x,
				 int lastY, lastX)        
{	
    int gid = get_global_id(0);
	__private result r;
	sudokuOut[gid] = sudokuIn[lastPallet[gid]];
	sudokuOut[gid].n[lastY][lastX] = last[gid];
	__private sudokuPallet pal = sudokuOut[gid];

	for (int i = 1; i < 10; i++)
		r.r[i] = 0;
	//printf("%d %d %d %d", sizeof(sudokuPallet), sizeof(result), gid, get_local_id(0));
	//printf("%d %d %d %d %d ", pal.n[0][0], pal.n[0][1], pal.n[0][2], pal.n[0][3], pal.n[0][4]);
	for (int i = 0; i < 9; i++)
		r.r[pal.n[i][x]] = 1;
	for (int i = 0; i < 9; i++)
		r.r[pal.n[y][i]] = 1;
	int xkw = (x / 3) * 3;
	int ykw = (y / 3) * 3;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			r.r[pal.n[ykw + i][xkw + j]] = 1;
		}
	r.r[0] = !r.r[1] + !r.r[2] + !r.r[3] + !r.r[4] + !r.r[5] +
		!r.r[6] + !r.r[7] + !r.r[8] + !r.r[9];
	output[gid] = r;
	
}

