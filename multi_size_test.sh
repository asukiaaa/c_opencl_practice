#!/bin/bash

DIR_PATH=`dirname $0`

GPU_SOURCE=${DIR_PATH}/matrix_dot_matrix.c
GPU_EXE=${DIR_PATH}/gpu.out
CPU_SOURCE=${DIR_PATH}/matrix_dot_matrix_on_cpu.c
CPU_EXE=${DIR_PATH}/cpu.out
DATETIME=`date "+%Y%m%d-%H%M%S"`
RESULT_TXT=${DIR_PATH}/${DATETIME}-result.txt

#touch $RESULT_TXT

gcc $GPU_SOURCE -lOpenCL -o $GPU_EXE
gcc $CPU_SOURCE -o $CPU_EXE

for SIZE in 4 16 64 256 1024 4096 16384
do
  echo "For ${SIZE}"
  echo "For ${SIZE}" >> $RESULT_TXT
  echo "CPU"
  echo "CPU" >> $RESULT_TXT
  $CPU_EXE $SIZE >> $RESULT_TXT
  echo "" >> $RESULT_TXT
  echo "GPU"
  echo "GPU" >> $RESULT_TXT
  $GPU_EXE $SIZE >> $RESULT_TXT
  echo "" >> $RESULT_TXT
  echo "" >> $RESULT_TXT
done
