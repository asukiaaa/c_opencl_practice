# opencl c practice

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

## Matrix dot matrix
```
gcc matrix_dot_matrix.c -lOpenCL
./a.out
```


# References
- [3.3 First OpenCL Program](https://www.fixstars.com/en/opencl/book/OpenCLProgrammingBook/first-opencl-program/)
- [How to compile OpenCL example in GCC?](https://forums.khronos.org/showthread.php/5728-How-to-compile-OpenCL-example-in-GCC)
- [Matrix multiplication in OpenCL](http://www.es.ele.tue.nl/~mwijtvliet/5KK73/?page=mmopencl)
- [KhronosGroup/OpenCL-Headers/opencl22/CL/cl.h](https://github.com/KhronosGroup/OpenCL-Headers/blob/master/opencl22/CL/cl.h)
