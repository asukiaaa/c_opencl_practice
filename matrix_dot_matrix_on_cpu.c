#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

double getSeconds(clock_t from, clock_t till) {
  return (double) (till - from) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
  clock_t start_t, set_matrix_t, end_t;

  start_t = clock();
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
  unsigned int matrixAMemSize = sizeof(float) * wA * hA;
  unsigned int matrixBMemSize = sizeof(float) * wB * hB;
  unsigned int matrixRMemSize = sizeof(float) * wR * hR;
  float* matrixA = (float*) malloc(matrixAMemSize);
  float* matrixB = (float*) malloc(matrixBMemSize);
  float* matrixR = (float*) malloc(matrixRMemSize);

  int i, j, k;
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
  set_matrix_t = clock();

  for (i = 0; i < wA; i++) {
    for (j = 0; j < wB; j++) {
      float value = 0;
      for (k = 0; k < wA; k++) {
        float elementA = matrixA[j * wA + k];
        float elementB = matrixB[k * wB + i];
        value += elementA * elementB;
      }
      matrixR[j*wB + i] = value;
    }
  }

  end_t = clock();
  if (wA <= 10)
    printMatrix(matrixR, wR, hR);

  printf("Set matrix time: %f sec\n", getSeconds(start_t, set_matrix_t));
  printf("Calc time: %f sec\n", getSeconds(set_matrix_t, end_t));
  printf("Total time: %f seconds\n", getSeconds(start_t, end_t));

  free(matrixA);
  free(matrixB);
  free(matrixR);

  return 0;
}
