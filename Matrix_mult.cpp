#include "MatrixMultiplication.h"

void mmult(const matrix_type Input_1[MATRIX_WIDTH * MATRIX_WIDTH],
		const matrix_type Input_2[MATRIX_WIDTH * MATRIX_WIDTH],
		matrix_type Output[MATRIX_WIDTH * MATRIX_WIDTH]) {
#pragma HLS INTERFACE m_axi port=Input_1 bundle=aximm1
#pragma HLS INTERFACE m_axi port=Input_2 bundle=aximm2
#pragma HLS INTERFACE m_axi port=Output bundle=aximm1
	matrix_type Buffer_1[MATRIX_WIDTH][MATRIX_WIDTH];
	matrix_type Buffer_2[MATRIX_WIDTH][MATRIX_WIDTH];

	/*
	 * Partitions an array into smaller arrays or
	 * individual elements and provides the following:
	 * Effectively increases the amount of read and
	 * write ports for the storage.
	 *
	 * Cyclic reshaping creates smaller arrays by
	 * interleaving elements from the original array
	 */
	#pragma HLS ARRAY_RESHAPE variable=Buffer_1 cyclic factor=32 dim=0
	#pragma HLS ARRAY_RESHAPE variable=Buffer_2 cyclic factor=32 dim=0

	Init_loop_i: for (int i = 0; i < MATRIX_WIDTH; i++)
		Init_loop_j: for (int j = 0; j < MATRIX_WIDTH; j++) {
			Buffer_1[i][j] = Input_1[i * MATRIX_WIDTH + j];
			Buffer_2[i][j] = Input_2[i * MATRIX_WIDTH + j];
		}

	Main_loop_i: for (int i = 0; i < MATRIX_WIDTH; i++)
		Main_loop_j: for (int j = 0; j < MATRIX_WIDTH; j++) {

			/*
			 * pipeline the Main_loop_j loop with the
			 * minimal initiation interval (II) of 1
			 * using the pipeline pragma
			 *
			 * Clock period also set to 8ns from
			 * solution settings
			 */
			#pragma HLS pipeline II=1

			matrix_type Result = 0;
			Main_loop_k: for (int k = 0; k < MATRIX_WIDTH - 1; k+=2) {
				Result += Buffer_1[i][k]   * Buffer_2[k][j];
				Result += Buffer_1[i][k+1] * Buffer_2[k+1][j];
			}

			Output[i * MATRIX_WIDTH + j] = Result;
		}
}
