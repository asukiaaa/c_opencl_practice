__kernel void matrix_dot_matrix(
  __global float *A,
  __global float *B,
  __global float *Result,
  int wA,
  int wB) {
  int tx = get_global_id(0);
  int ty = get_global_id(1);

  float value = 0;
  for (int k = 0; k < wA; ++k) {
    float elementA = A[ty * wA + k];
    float elementB = B[k * wB + tx];
    value += elementA * elementB;
  }

  Result[ty * wA + tx] = value;
}
