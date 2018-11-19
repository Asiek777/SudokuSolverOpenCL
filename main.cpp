/**********************************************************************
Copyright ©2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#define  _VARIADIC_MAX 10
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <functional>
#include <time.h>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <vector>
#include "sudokuPallet.h"

//#define SUCCESS 0
//#define FAILURE 1
//#define EXPECTED_FAILURE 2

const int NUM_ELEMENTS = 64;
const size_t palletSize = sizeof(sudokuPallet);
clock_t start, end;

typedef struct _result {
	cl_char r[10];
} result;

void printResult(std::string header, result r);

typedef cl::make_kernel<cl::Buffer&, cl::Buffer&,
	cl_int&, cl_int&> KernelType;

void prepareNewBuffer(cl::Buffer& oldInputBuffer, cl::Buffer& oldOutputBuffer, 
	cl::Buffer& newInputBuffer, cl::Buffer& newOutputBuffer, int y, int x, int& size) {
	std::vector<result> output(size);
	cl::copy(oldOutputBuffer, output.begin(), output.end());
	int newSize = 0, smallOffset = y * 9 + x;

	for (int i = 0; i < size; i++)
		newSize += output[i].r[0];

	newInputBuffer = cl::Buffer(CL_MEM_READ_WRITE, palletSize*newSize);
	newOutputBuffer = cl::Buffer(CL_MEM_READ_WRITE, sizeof(result)*newSize);

	int count = 0;
	cl::CommandQueue queue = cl::CommandQueue::getDefault();
	for (int i = 0; i < size; i++) {
		for (char j = 1; j < 10; j++) {
			if (!output[i].r[j]) {
				clEnqueueCopyBuffer(queue(), oldInputBuffer(), newInputBuffer(),
					palletSize*i, palletSize*count, palletSize, 0, NULL, NULL);
				clEnqueueWriteBuffer(queue(), newInputBuffer(), CL_TRUE,
					palletSize * count + smallOffset, sizeof(char), &j, 0, NULL, NULL);
				count++;
			}
		}		
	}
	size = newSize;
}

sudokuPallet solveSudoku(sudokuPallet& pallet, KernelType& vectorAddKernel) {
	std::vector<sudokuPallet> input(1);
	input[0] = pallet;
	std::vector<result> output(1);
	int x = -1, y = 0;
	int  size = 1;
	cl::Buffer newInputBuffer(input.begin(), input.end(), true);
	cl::Buffer newOutputBuffer(output.begin(), output.end(), false);
	cl::Buffer oldInputBuffer, oldOutputBuffer;
	while (!next0inPallet(pallet, y, x)) {
		cl::Event e;
		std::cout << "size: ";

		cl::NDRange ndrg(size);
		cl::NDRange ndrl(size > 256 ? 256 : size);
		cl::EnqueueArgs arg(ndrg, ndrl);
		std::cout << size << std::endl;

		e = vectorAddKernel(arg,
			newOutputBuffer, newInputBuffer,
			y, x);

		end = clock();
		std::cout << "time: " << (double)(end - start)/1000 << std::endl;

		cl::copy(newOutputBuffer, output.begin(), output.end());
		//printResult("output", output[0]);

		oldInputBuffer = newInputBuffer;
		oldOutputBuffer = newOutputBuffer;
		prepareNewBuffer(oldInputBuffer, oldOutputBuffer, newInputBuffer,
			newOutputBuffer, y, x, size);

		cl::copy(newInputBuffer, input.begin(), input.end());

		//printPallet("hope", input[0]);

	}

	return input[0];
}


int 
main(int argc, char * argv[]) {
	cl_int status = 0;
	const char *filename = "sudokuSolver.cl";
	std::string sourceStr;
	start = clock();

	sudokuPallet pallet = fileToSudoku("example/Entry.txt");
	status = fileToString(filename, sourceStr);
	if (status != SUCCESS) {
		std::cout << "Failed to open " << filename << std::endl;
		return FAILURE;
	}

	printPallet("input:",pallet);

	cl::Program vectorAddProgram(std::string(sourceStr), true);

	KernelType vectorAddKernel(vectorAddProgram, "vectorAdd");

	std::vector<sudokuPallet> input(1);
	input[0] = pallet;
	std::vector<result> output(1);

	sudokuPallet solved = solveSudoku(pallet, vectorAddKernel);
		/*cl::Buffer inputBuffer(input.begin(), input.end(), isReadOnly);
		cl::Buffer outputBuffer(output.begin(), output.end(), !isReadOnly);

		cl::Event e;
		cl::Platform platform = cl::Platform::getDefault();

		cl::NDRange ndrg(1);
		cl::NDRange ndrl(1);
		int x = 2, y = 1;
		cl::EnqueueArgs arg(ndrg, ndrl);

		e = vectorAddKernel(
			arg,
			outputBuffer,
			inputBuffer,
			x,
			y);*/

	printPallet("\nDone:", solved);
	printPallet("\nInput:", pallet);
	std::cout << "Passed\n";
	

    return SUCCESS;
}

void printResult(std::string header, result r) {
	std::cout << header << std::endl;
	for (int i = 0; i < 10; i++) {
		std::cout << (int)r.r[i] << " ";
	}
	std::cout << std::endl;
}

