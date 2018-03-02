#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

int getMaxCommonFactorOf2Pow(int target) {
  int commonFactor = 1;
  while (target % 2 == 0) {
    target /= 2;
    commonFactor *= 2;
  }
  return commonFactor;
}

double getSeconds(clock_t from, clock_t till) {
  return (double) (till - from) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
  clock_t start_t, loaded_device_t, set_kernel_t, set_memory_t, set_work_size_t, end_calc_t, end_t;

  int wA, hA, wB;
  if (argc == 1) {
    wA = hA = wB = 10;
  } else if (argc == 2) {
    wA = hA = wB = atoi(argv[1]);
  } else if (argc == 4) {
    wA = atoi(argv[1]);
    hA = atoi(argv[2]);
    wB = atoi(argv[3]);
  } else {
    printf("No value or 1 or 3 values for wA, hA and wB are expected.\n");
    return EXIT_FAILURE;
  }
  int hB = wA, wR = wB, hR = hA;
  unsigned int matrixAMemSize = sizeof(float) * (unsigned int) (wA * hA);
  unsigned int matrixBMemSize = sizeof(float) * (unsigned int) (wB * hB);
  unsigned int matrixRMemSize = sizeof(float) * (unsigned int) (wR * hR);
  float* matrixA = (float*) malloc(matrixAMemSize);
  float* matrixB = (float*) malloc(matrixBMemSize);
  float* matrixR = (float*) malloc(matrixRMemSize);

  int i, j;
  for (i = 0; i < wA; i++) {
    for (j = 0; j < hA; j++) {
      matrixA[j*wA + i] = 10*i + j;
    }
  }
  for (i = 0; i < wB; i++) {
    for (j = 0; j < hB; j++) {
      int bValue = 0;
      if (i == j) {
        if (i == 1) {
          bValue = 2;
        } else {
          bValue = 1;
        }
      }
      matrixB[j*wB + i] = bValue;
    }
  }

  if (wA <= 10) {
    printMatrix(matrixA, wA, hA);
    printMatrix(matrixB, wB, hB);
  }
  start_t = clock();

  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem matrixAMemObj = NULL, matrixBMemObj = NULL, matrixRMemObj = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;

  /* Get Platform and Device Info */
  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  if (ret != CL_SUCCESS) {
    printf("Error: Cannot get platform id!\n");
    return EXIT_FAILURE;
  }
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }
  loaded_device_t = clock();

  /* Set source code of kernel*/
  char *source_str =
    "__kernel void matrix_dot_matrix( \
       __global float *A, \
       __global float *B, \
       __global float *Result, \
       int wA, \
       int wB) { \
       int tx = get_global_id(0); \
       int ty = get_global_id(1); \
       float value = 0; \
       for (int k = 0; k < wA; ++k) { \
         float elementA = A[ty * wA + k]; \
         float elementB = B[k * wB + tx]; \
         value += elementA * elementB; \
       } \
       Result[ty * wB + tx] = value; \
     }";
  size_t source_size = strlen(source_str);

  /* Create OpenCL context */
  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

  /* Create Command Queue */
  command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);

  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

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
  set_kernel_t = clock();

  /* Create Memory Buffer */
  matrixAMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, matrixAMemSize, matrixA, &ret);
  matrixBMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, matrixBMemSize, matrixB, &ret);
  matrixRMemObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixRMemSize, NULL, &ret);
  if (!matrixAMemObj || !matrixBMemObj || !matrixRMemObj) {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
  }

  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&matrixAMemObj);
  ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&matrixBMemObj);
  ret |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&matrixRMemObj);
  ret |= clSetKernelArg(kernel, 3, sizeof(int), (void *)&wA);
  ret |= clSetKernelArg(kernel, 4, sizeof(int), (void *)&wB);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to set kernel arguments! %d\n", ret);
    exit(1);
  }
  set_memory_t = clock();

  /* Get max work item sizes */
  size_t maxWorkGroupSize;
  {
    ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
    if (ret != CL_SUCCESS) {
      printf("Error: Failed to get workgroup_size %d\n", ret);
      return EXIT_FAILURE;
    }
    printf("CL_DEVICE_MAX_WORK_GROUP_SIZE: %lu\n", maxWorkGroupSize);
  }
  size_t* maxLocalSizes;
  {
    cl_uint maxWorkItemDims;
    ret = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maxWorkItemDims, NULL);
    if (ret != CL_SUCCESS) {
      printf("Error: Failed to get work_item_dims %d\n", ret);
      return EXIT_FAILURE;
    }
    maxLocalSizes = (size_t*)malloc(maxWorkItemDims * sizeof(size_t));

    ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, maxWorkItemDims* sizeof(size_t), maxLocalSizes, NULL);
    if (ret != CL_SUCCESS) {
      printf("Error: Failed to get maxLocalSizes %d\n", ret);
      return EXIT_FAILURE;
    }
    printf("work item sizes: ");
    for (i = 0; i< maxWorkItemDims; i++)
      printf(" %ld", maxLocalSizes[i]);
    printf("\n");
  }

  /* Decide work size */
  size_t localWR = wR, localHR = hR;
  while (maxLocalSizes[0] < localWR ||
         maxLocalSizes[1] < localHR ||
         localWR * localHR > maxWorkGroupSize) {
    if (maxLocalSizes[0] < localWR) {
      if (localWR % 2 == 0) {
        localWR /= 2;
      } else {
        localWR = getMaxCommonFactorOf2Pow(localWR);
      }
    } else if (maxLocalSizes[1] < localHR) {
      if (localWR % 2 == 0) {
        localHR /= 2;
      } else {
        localHR = getMaxCommonFactorOf2Pow(localHR);
      }
    } else if (localWR > localHR && localWR % 2 == 0) {
      localWR /= 2;
    } else if (localHR % 2 == 0) {
      localHR /= 2;
    } else if (localWR % 2 == 0) {
      localWR /= 2;
    } else if (localHR != 1) {
      localHR = getMaxCommonFactorOf2Pow(localHR);
    } else {
      localWR = getMaxCommonFactorOf2Pow(localWR);
    }
  }
  int workDim = 1;
  size_t globalWorkSize[] = {wR * hR};
  size_t localWorkSize[] = {localWR};
  printf("localWorkSize: %ld, %ld\n", localWorkSize[0], localWorkSize[1]);
  set_work_size_t = clock();

  /* Execute OpenCL Kernel */
  ret = clEnqueueNDRangeKernel(command_queue, kernel, workDim, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    printf("Error: Failed to execute kernel! %d\n", ret);
    exit(1);
  }

  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(command_queue, matrixRMemObj, CL_TRUE, 0, matrixRMemSize, matrixR, 0, NULL, NULL);
  end_calc_t = clock();

  /* Finalization */
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(matrixAMemObj);
  ret = clReleaseMemObject(matrixBMemObj);
  ret = clReleaseMemObject(matrixRMemObj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  end_t = clock();

  /* Display Result */
  if (wA <= 10) {
    printMatrix(matrixR, wR, hR);
  }

  /* Show time */
  printf("Load device time: %f sec\n", getSeconds(start_t, loaded_device_t));
  printf("Set kernel time: %f sec\n", getSeconds(loaded_device_t, set_kernel_t));
  printf("Set memory time: %f sec\n", getSeconds(set_kernel_t, set_memory_t));
  printf("Set work size time: %f sec\n", getSeconds(set_memory_t, set_work_size_t));
  printf("Calc time: %f sec\n", getSeconds(set_work_size_t, end_calc_t));
  printf("Release resource time: %f sec\n", getSeconds(end_calc_t, end_t));
  printf("Total time: %f sec\n", getSeconds(start_t, end_t));

  free(matrixA);
  free(matrixB);
  free(matrixR);
  free(maxLocalSizes);

  return 0;
}
