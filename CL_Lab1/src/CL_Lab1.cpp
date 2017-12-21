//============================================================================
// Name        : CL_Lab3.cpp
// Author      : SmallSharky
// Version     :
// Copyright   : GNU GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <stdio.h>
#include <cmath>

using namespace std;

std::string clke_descr(cl_int err) {
	string ret;
	switch (err) {
		case CL_SUCCESS: {
			ret = "OK";
			break;
		}

		case CL_INVALID_KERNEL: {
			ret = "Invalid kernel";
			break;
		}

		case CL_INVALID_KERNEL_ARGS: {
			ret = "Invalid kernel args";
			break;
		}
		case CL_INVALID_ARG_INDEX: {
			ret = "Invalid arg index";
			break;
		}
		case CL_INVALID_ARG_VALUE: {
			ret = "Invalid arg value";
			break;
		}
		case CL_INVALID_MEM_OBJECT: {
			ret = "Invalid mem obj";
			break;
		}
		case CL_INVALID_SAMPLER: {
			ret = "Invalid sampler";
			break;
		}
		case CL_INVALID_ARG_SIZE: {
			ret = "Invalid arg size";
			break;
		}
		case CL_INVALID_PROGRAM: {
			ret = "CL_INVALID_PROGRAM";
			break;
		}
		case CL_INVALID_VALUE: {
			ret = "CL_INVALID_VALUE";
			break;
		}
		case CL_INVALID_DEVICE: {
			ret = "CL_INVALID_DEVICE";
			break;
		}
		case CL_INVALID_BINARY: {
			ret = "CL_INVALID_BINARY";
			break;
		}
		case CL_INVALID_BUILD_OPTIONS: {
			ret = "CL_INVALID_BUILD_OPTIONS";
			break;
		}
		case CL_INVALID_OPERATION: {
			ret = "CL_INVALID_OPERATION";
			break;
		}
		case CL_COMPILER_NOT_AVAILABLE: {
			ret = "CL_COMPILER_NOT_AVAILABLE";
			break;
		}
		case CL_BUILD_PROGRAM_FAILURE: {
			ret = "CL_BUILD_PROGRAM_FAILURE";
			break;
		}
		case CL_OUT_OF_HOST_MEMORY: {
			ret = "CL_OUT_OF_HOST_MEMORY";
			break;
		}

		default: {
			ret = "Uknown err";
			break;
		}
	}
	return ret;
}

int main() {
	/* получить доступные платформы */
	cl_platform_id platform_id;
	unsigned int ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	cout << "Num of platforms: " << ret_num_platforms << "\n";

	/* получить доступные устройства */
	cl_device_id device_id;
	unsigned int ret_num_devices;
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	cout << "Num of devices: " << ret_num_devices << "\n";
	/* создать контекст */
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cout << "Create context: " << clke_descr(ret) << "\n";
	/* создаем команду */
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	cout << "Create queue: " << clke_descr(ret) << "\n";

	cl_program program = NULL;
	cl_kernel kernel = NULL;
	FILE *fp;
	const char fileName[] = "src/test.ocl";
	size_t source_size;
	string source_str = "";
	try {
		fp = fopen(fileName, "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			return 1;
		}
		//source_str = (char *)malloc(MAX_SOURCE_SIZE);
		while (!feof(fp)) {
			char c;
			fread(&c, 1, 1, fp);
			source_str += c;

		}
		fclose(fp);
	} catch (...) {
		printf("EXCEPTION\n");
	}
	cout << "Get code: ";
	const char * sstrptr = source_str.c_str();
	source_size = source_str.length();
	//cout<<"--------------------------------------\n";
	//cout<<source_str<<"\n";
	//cout<<"--------------------------------------\n";

	/* создать бинарник из кода программы */
	cout << "OK\n";
	//cout << "Code ptr: " << (void *) sstrptr << "\n";
	program = clCreateProgramWithSource(context, 1, &sstrptr, (const size_t *) &source_size, &ret);
	cout << "create program: " << clke_descr(ret) << "\n";
	/* скомпилировать программу */

	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	cout << "compile: " << clke_descr(ret) << "\n";
	if (ret) return -110;
	/* создать кернел */
	kernel = clCreateKernel(program, "test", &ret);
	cout << "mk kernel: " << clke_descr(ret) << "\n";

	/*
	 * Готовим векторы, выделяем память
	 * */
	cl_mem x_mo;
	x_mo = NULL;
	size_t mem_len = 100;

	cl_float *x_mem = new cl_float[mem_len];

	size_t i_mem = 0;
	while (i_mem < mem_len) {
		x_mem[i_mem] = 3*(M_PI/2);
		i_mem++;
	}

	cout << "mkbuffers:\n";
	//Кидаем данные на видеокарту
	x_mo = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_len * sizeof(cl_float), NULL, &ret);
	cout << "\tx: " << clke_descr(ret) << "\n";

	cout << "writebuf:\n";
	ret = clEnqueueWriteBuffer(command_queue, x_mo, CL_TRUE, 0, mem_len * sizeof(cl_float), x_mem, 0, NULL, NULL);
	cout << "\tx: " << clke_descr(ret) << "\n";

	cout << "set kernargs:\n";
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &x_mo);
	cout << "\tx: " << clke_descr(ret) << "\n";

	size_t global_work_size[1] = { mem_len };

	/* выполнить кернел */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	cout << "launch: " << clke_descr(ret) << "\n";
	/* считать данные из буфера */
	ret = clEnqueueReadBuffer(command_queue, x_mo, CL_TRUE, 0, mem_len * sizeof(cl_float), x_mem, 0, NULL, NULL);

	size_t i = 0;

	while (i < mem_len) {

		cout << "R[" << i << "]: " << x_mem[i] << "\n";

		i++;
	}
	return 0;
}
