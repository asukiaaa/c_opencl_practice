# c opencl practice

# Environment

Name | Version
---- | ----
OS | Ubuntu 17.04
CPU | Intel Corei7-4600U CPU @ 2.10GHz × 4
GPU | Intel Haswell Mobile
GPU driver | [OpenCL™ Runtime for Intel® Core™ and Intel® Xeon® Processors](https://software.intel.com/en-us/articles/opencl-drivers) ver 16.1.2 for Red Hat and Ubuntu Linux
OpenCL | 2.2.11-1ubuntu1
GCC | 7.2.0-8ubuntu3

# Usage

## Hello world
```
gcc hello.c -lOpenCL
./a.out
```

## Matrix dot matrix on GPU
```
gcc matrix_dot_matrix.c -lOpenCL
./a.out
```

## Matrix dot matrix on CPU
```
gcc matrix_dot_matrix_on_cpu.c
./a.out
```

# Times

Matrixes mutation.

Program | 2000x2000 | 4000x4000 | 10000x10000
---- | ---- | ---- | ----
matrix_dot_matrix_on_cpu | 62.441596 seconds | 594.915341 seconds | Did not try
matrix_dot_matrix (on GPU) | 18.725523 seconds | 203.470782 seconds | 4436.742463 seconds
clEnqueueNDRangeKernel | 0.000044 seconds | 0.000046 seconds | 0.000047 seconds

# References
- [3.3 First OpenCL Program](https://www.fixstars.com/en/opencl/book/OpenCLProgrammingBook/first-opencl-program/)
- [How to compile OpenCL example in GCC?](https://forums.khronos.org/showthread.php/5728-How-to-compile-OpenCL-example-in-GCC)
- [Matrix multiplication in OpenCL](http://www.es.ele.tue.nl/~mwijtvliet/5KK73/?page=mmopencl)
- [KhronosGroup/OpenCL-Headers/opencl22/CL/cl.h](https://github.com/KhronosGroup/OpenCL-Headers/blob/master/opencl22/CL/cl.h)
- [clEnqueueNDRangeKernel](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueNDRangeKernel.html)
