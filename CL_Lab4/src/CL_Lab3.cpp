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
using namespace std;

std::string clke_descr(cl_int err)
{
	string ret;
	switch (err)
	{
	case CL_SUCCESS:
	{
		ret = "OK";
		break;
	}

	case CL_INVALID_KERNEL:
	{
		ret = "Invalid kernel";
		break;
	}

	case CL_INVALID_KERNEL_ARGS:
	{
		ret = "Invalid kernel args";
		break;
	}
	case CL_INVALID_ARG_INDEX:
	{
		ret = "Invalid arg index";
		break;
	}
	case CL_INVALID_ARG_VALUE:
	{
		ret = "Invalid arg value";
		break;
	}
	case CL_INVALID_MEM_OBJECT:
	{
		ret = "Invalid mem obj";
		break;
	}
	case CL_INVALID_SAMPLER:
	{
		ret = "Invalid sampler";
		break;
	}
	case CL_INVALID_ARG_SIZE:
	{
		ret = "Invalid arg size";
		break;
	}
	case CL_INVALID_PROGRAM:
	{
		ret = "CL_INVALID_PROGRAM";
		break;
	}
	case CL_INVALID_VALUE:
	{
		ret = "CL_INVALID_VALUE";
		break;
	}
	case CL_INVALID_DEVICE:
	{
		ret = "CL_INVALID_DEVICE";
		break;
	}
	case CL_INVALID_BINARY:
	{
		ret = "CL_INVALID_BINARY";
		break;
	}
	case CL_INVALID_BUILD_OPTIONS:
	{
		ret = "CL_INVALID_BUILD_OPTIONS";
		break;
	}
	case CL_INVALID_OPERATION:
	{
		ret = "CL_INVALID_OPERATION";
		break;
	}
	case CL_COMPILER_NOT_AVAILABLE:
	{
		ret = "CL_COMPILER_NOT_AVAILABLE";
		break;
	}
	case CL_BUILD_PROGRAM_FAILURE:
	{
		ret = "CL_BUILD_PROGRAM_FAILURE";
		break;
	}
	case CL_OUT_OF_HOST_MEMORY:
	{
		ret = "CL_OUT_OF_HOST_MEMORY";
		break;
	}


	default:
	{
		ret = "Uknown err";
		break;
	}
	}
	return ret;
}

int main()
{
	/* получить доступные платформы */
	cl_platform_id platform_id;
	unsigned int ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	cout << "Num of platforms: " << ret_num_platforms << "\n";

	/* получить доступные устройства */
	cl_device_id device_id;
	unsigned int ret_num_devices;
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
			&ret_num_devices);
	cout << "Num of devices: " << ret_num_devices << "\n";
	/* создать контекст */
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* создаем команду */
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0,
			&ret);

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
	//cout<<"--------------------------------------\n";
	//cout<<source_str<<"\n";
	//cout<<"--------------------------------------\n";

	/* создать бинарник из кода программы */

	program = clCreateProgramWithSource(context, 1, &sstrptr,
			(const size_t *) &source_size, &ret);
	cout << "create program: " << clke_descr(ret) << "\n";
	/* скомпилировать программу */

	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	cout << "compile: " << clke_descr(ret) << "\n";
	/* создать кернел */
	kernel = clCreateKernel(program, "test", &ret);
	cout << "mk kernel: " << clke_descr(ret) << "\n";

	/*
	 * Готовим векторы, выделяем память
	 * */
	cl_mem u_mo, p_mo, a_mo, b_mo, c_mo;
	u_mo = p_mo = a_mo = b_mo = c_mo = NULL;
	size_t mem_len = 100;

	cl_float3 *vec_u_mem = new cl_float3[mem_len];
	cl_float3 *dot_p_mem = new cl_float3[mem_len];
	cl_float3 *dot_a_mem = new cl_float3[mem_len];
	cl_float3 *dot_b_mem = new cl_float3[mem_len];
	cl_float3 *dot_c_mem = new cl_float3[mem_len];

	size_t i_mem = 0;
	while (i_mem < mem_len)
	{
		vec_u_mem[i_mem].x = 1000;
		vec_u_mem[i_mem].y = 1000;
		vec_u_mem[i_mem].z = 1000;
		dot_p_mem[i_mem].x = 0;
		dot_p_mem[i_mem].y = 0;
		dot_p_mem[i_mem].z = 0;
		dot_a_mem[i_mem].x = 100;
		dot_a_mem[i_mem].y = 0;
		dot_a_mem[i_mem].z = 0;
		dot_b_mem[i_mem].x = 0;
		dot_b_mem[i_mem].y = 100;
		dot_b_mem[i_mem].z = 0;
		dot_c_mem[i_mem].x = 0;
		dot_c_mem[i_mem].y = 0;
		dot_c_mem[i_mem].z = 100;

		i_mem++;
	}

	cout << "mkbuffers:\n";
	//Кидаем данные на видеокарту
	u_mo = clCreateBuffer(context, CL_MEM_READ_WRITE,
			mem_len * sizeof(cl_float3), NULL, &ret);
	cout << "u: " << clke_descr(ret) << "\n";
	p_mo = clCreateBuffer(context, CL_MEM_READ_WRITE,
			mem_len * sizeof(cl_float3), NULL, &ret);
	cout << "p: " << clke_descr(ret) << "\n";
	a_mo = clCreateBuffer(context, CL_MEM_READ_WRITE,
			mem_len * sizeof(cl_float3), NULL, &ret);
	cout << "a: " << clke_descr(ret) << "\n";
	b_mo = clCreateBuffer(context, CL_MEM_READ_WRITE,
			mem_len * sizeof(cl_float3), NULL, &ret);
	cout << "b: " << clke_descr(ret) << "\n";
	c_mo = clCreateBuffer(context, CL_MEM_READ_WRITE,
			mem_len * sizeof(cl_float3), NULL, &ret);
	cout << "c: " << clke_descr(ret) << "\n";
	cout << "writebuf:\n";
	ret = clEnqueueWriteBuffer(command_queue, u_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), vec_u_mem, 0, NULL, NULL);
	cout << "u: " << clke_descr(ret) << "\n";
	ret = clEnqueueWriteBuffer(command_queue, p_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), dot_p_mem, 0, NULL, NULL);
	cout << "p: " << clke_descr(ret) << "\n";
	ret = clEnqueueWriteBuffer(command_queue, a_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), dot_a_mem, 0, NULL, NULL);
	cout << "a: " << clke_descr(ret) << "\n";
	ret = clEnqueueWriteBuffer(command_queue, b_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), dot_b_mem, 0, NULL, NULL);
	cout << "b: " << clke_descr(ret) << "\n";
	ret = clEnqueueWriteBuffer(command_queue, c_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), dot_c_mem, 0, NULL, NULL);
	cout << "c: " << clke_descr(ret) << "\n";

	cout << "set kernargs:\n";
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &u_mo);
	cout << "u: " << clke_descr(ret) << "\n";

	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &p_mo);
	cout << "p: " << clke_descr(ret) << "\n";

	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &a_mo);
	cout << "a: " << clke_descr(ret) << "\n";

	ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *) &b_mo);
	cout << "b: " << clke_descr(ret) << "\n";

	ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *) &c_mo);
	cout << "c: " << clke_descr(ret) << "\n";

	size_t global_work_size[1] =
	{ mem_len };
	cout << "launch\n";
	/* выполнить кернел */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
			global_work_size, NULL, 0, NULL, NULL);

	/* считать данные из буфера */
	ret = clEnqueueReadBuffer(command_queue, u_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), vec_u_mem, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, p_mo, CL_TRUE, 0,
			mem_len * sizeof(cl_float3), dot_p_mem, 0, NULL, NULL);
	size_t i = 0;

	while (i < mem_len)
	{
		cout << "V[" << i << "]: " << vec_u_mem[i].x <<", "<< vec_u_mem[i].y <<", "<< vec_u_mem[i].z <<" : ";
		cout << dot_p_mem[i].x<<", "<<dot_p_mem[i].y<<", "<<dot_p_mem[i].z<<"\n";
		i++;
	}
	return 0;
}
