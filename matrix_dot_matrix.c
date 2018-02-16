#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void printMatrix(float* matrix, int w, int h) {
  int i, j;
  printf("matrix\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      printf(" %6.2f", matrix[i*w + j]);
    }
    printf("\n");
  }
}

int main() {
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem matrixAMemObj = NULL, matrixBMemObj = NULL, matrixResultMemObj = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;

  FILE *fp;
  char fileName[] = "./matrix_dot_matrix.cl";
  char *source_str;
  size_t source_size;

  const int wA = 10, wB = 10;
  //const int wA = 2000, wB = 2000;
  unsigned int matrixSize = wA * wB;
  unsigned int matrixMemSize = sizeof(float) * matrixSize;
  float* matrixA = (float*) malloc(matrixMemSize);
  float* matrixB = (float*) malloc(matrixMemSize);
  float* matrixResult = (float*) malloc(matrixMemSize);

  int i, j;
  for (i = 0; i < wA; i++) {
    for (j = 0; j < wB; j++) {
      matrixA[j*wA + i] = 10*i + j;
      int bValue = 0;
      if (i == j) {
        if (i == 1) {
          bValue = 2;
        } else {
          bValue = 1;
        }
      }
      matrixB[i*wB + j] = bValue;
    }
  }

  printMatrix(matrixA, wA, wB);
  printMatrix(matrixB, wB, wA);

  /* Load the source code containing the kernel*/
  fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  /* Get Platform and Device Info */
  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  /* Create OpenCL context */
  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

  /* Create Command Queue */
  command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);

  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                      (const size_t *)&source_size, &ret);

  /* Build Kernel Program */
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  if (ret != CL_SUCCESS) {
    size_t len;
    char buffer[2048];
    printf("Error: Failed to build program executable!\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(1);
  }

  /* Create OpenCL Kernel */
  kernel = clCreateKernel(program, "matrix_dot_matrix", &ret);
  if (!kernel || ret != CL_SUCCESS) {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
  }

  /* Create Memory Buffer */
  matrixAMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, matrixMemSize, matrixA, &ret);
  matrixBMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, matrixMemSize, matrixB, &ret);
  matrixResultMemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixMemSize, NULL, &ret);
  if (!matrixAMemObj || !matrixBMemObj || !matrixResultMemObj) {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
  }

  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&matrixAMemObj);
  ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&matrixBMemObj);
  ret |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&matrixResultMemObj);
  ret |= clSetKernelArg(kernel, 3, sizeof(int), (void *)&wA);
  ret |= clSetKernelArg(kernel, 4, sizeof(int), (void *)&wB);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to set kernel arguments! %d\n", ret);
    exit(1);
  }

  /* Execute OpenCL Kernel */
  int workDim = 2;
  size_t globalWorkSize[workDim], localWorkSize[workDim];
  globalWorkSize[0] = wA;
  globalWorkSize[1] = wB;
  localWorkSize[0] = wA < 16 ? wA : 16;
  localWorkSize[1] = wB < 16 ? wB : 16;
  ret = clEnqueueNDRangeKernel(command_queue, kernel, workDim, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to execute kernel! %d\n", ret);
    exit(1);
  }

  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(command_queue, matrixResultMemObj, CL_TRUE, 0, matrixMemSize, matrixResult, 0, NULL, NULL);

  /* Display Result */
  printMatrix(matrixResult, wA, wB);

  /* Finalization */
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(matrixAMemObj);
  ret = clReleaseMemObject(matrixBMemObj);
  ret = clReleaseMemObject(matrixResultMemObj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  free(matrixA);
  free(matrixB);
  free(matrixResult);

  return 0;
}
