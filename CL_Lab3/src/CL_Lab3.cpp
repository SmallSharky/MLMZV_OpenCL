//============================================================================
// Name        : CL_Lab3.cpp
// Author      : SmallSharky
// Version     :
// Copyright   : GNU GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <CL/cl.h>
using namespace std;

int main()
{
	/* получить доступные платформы */
	cl_platform_id platform_id;
	unsigned int ret_num_platforms;
	auto ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	cout << "Num of platforms: " << ret_num_platforms << "\n";

	/* получить доступные устройства */
	cl_device_id device_id;
	unsigned int ret_num_devices;
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
			&ret_num_devices);
	cout << "Num of devices: " << ret_num_devices << "\n";
	/* создать контекст */
	auto context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* создаем команду */
	auto command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	cl_program program = NULL;
	cl_kernel kernel = NULL;
	FILE *fp;
	const char fileName[] = "src/test.cl";
	size_t source_size;
	string source_str = "";
	try
	{
		fp = fopen(fileName, "r");
		if (!fp)
		{
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		//source_str = (char *)malloc(MAX_SOURCE_SIZE);
		while (!feof(fp))
		{
			char c;
			fread(&c, 1, 1, fp);
			source_str += c;

		}
		fclose(fp);
	} catch (...)
	{
		printf("EXCEPTION\n");
	}
	cout << "get code\n";
	const char * sstrptr = source_str.c_str();
	/* создать бинарник из кода программы */
	cout << "create program\n";
	program = clCreateProgramWithSource(context, 1, &sstrptr,
			(const size_t *) &source_size, &ret);
	/* скомпилировать программу */
	cout<<"compile\n";
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	/* создать кернел */
	kernel = clCreateKernel(program, "test", &ret);

	cl_mem memobj = NULL;
	int memLenth = 10;
	cl_int* mem = new cl_int[memLenth];
	cl_int* ret_mem = new cl_int[memLenth];
	int i0 = 0;
	while (i0 < memLenth)
	{
		mem[i0] = i0;
		i0++;
	}
	mem[0] = 65536;
	cout << "preparing launch\n";
	/* создать буфер */
	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,
			memLenth * sizeof(cl_int), NULL, &ret);

	/* записать данные в буфер */
	ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0,
			memLenth * sizeof(cl_int), mem, 0, NULL, NULL);

	/* устанавливаем параметр */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &memobj);

	/* устанавливаем параметр */
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &memobj);

	size_t global_work_size[1] =
	{ 10 };
	cout << "launch\n";
	/* выполнить кернел */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
			global_work_size, NULL, 0, NULL, NULL);

	/* считать данные из буфера */
	ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
			memLenth * sizeof(float), ret_mem, 0, NULL, NULL);
	size_t i = 0;

	while (i < memLenth)
	{
		if (ret_mem[i] >= 0)
		{
			cout << "log2(" << mem[i] << ") = " << ret_mem[i] << "\n";
		}
		else
		{
			cout << "Value " << mem[i] << " isn\'t a 2^k\n";
		}
		i++;
	}
	return 0;
}
