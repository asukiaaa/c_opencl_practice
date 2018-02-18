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

int main(int argc, char *argv[]) {
  clock_t start_t, end_t;

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
  start_t = clock();

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

  double total_t = (double) (end_t - start_t) / CLOCKS_PER_SEC;
  printf("Total time: %f seconds\n", total_t);

  free(matrixA);
  free(matrixB);
  free(matrixR);

  return 0;
}
