/**********************************************************************
Copyright �2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

�	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
�	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

//typedef struct __attribute__((packed)) sudokuPallet {
typedef struct _sudokuPallet {
	char n[9][9];
} sudokuPallet;

typedef struct _result {
	char r[10];
} result;

//int kwadratNumber(int a) {
//	return a / 3 * 3;
//}

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
	sudokuOut[gid]=sudokuIn[lastPallet[gid]];
	sudokuOut[gid].n[lastY][lastX] = last[gid];
	__private sudokuPallet pal = sudokuOut[gid];

	for (int i = 1; i < 10; i++)
		r.r[i] = 0;
	//printf("%d %d %d %d", sizeof(sudokuPallet), sizeof(result), gid, get_local_id(0));
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

