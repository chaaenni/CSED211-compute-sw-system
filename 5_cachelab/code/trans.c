/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"



int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M == 32 && N == 32) transpose_32(M, N, A, B);
    else if(M == 64 && N == 64) transpose_64(M, N, A, B);
    else if(M == 61 && N == 67) transpose_61(M, N, A, B);


    return;

}

void transpose_32(int M, int N, int A[N][M], int B[M][N]){
    int i, j, _i; //variable for iteration
    int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for(i = 0; i < M; i += 8){
        for(j = 0; j < N; j += 8){
            for(_i = i ; _i < i + 8; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];
                tmp5 = A[_i][j+4];
                tmp6 = A[_i][j+5];
                tmp7 = A[_i][j+6];
                tmp8 = A[_i][j+7];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
                B[j+4][_i] = tmp5;
                B[j+5][_i] = tmp6;
                B[j+6][_i] = tmp7;
                B[j+7][_i] = tmp8;
            }
        }
    }
    return;
}

void transpose_64_1(int M, int N, int A[N][M], int B[M][N]){
    int i, j, _i; //variable for iteration
    int tmp1, tmp2, tmp3, tmp4;
    for(i = 0; i < M; i += 8){
        for(j = 0; j < N; j += 8){
            for(_i = i ; _i < i + 4; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
            }
            for(_i = i ; _i < i + 4; _i++){
                tmp1 = A[_i][j+4];
                tmp2 = A[_i][j+5];
                tmp3 = A[_i][j+6];
                tmp4 = A[_i][j+7];

                B[j+4][_i] = tmp1;
                B[j+5][_i] = tmp2;
                B[j+6][_i] = tmp3;
                B[j+7][_i] = tmp4;
            }
            for(_i = i + 4; _i < i + 8; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
            }
            for(_i = i + 4; _i < i + 8; _i++){
                tmp1 = A[_i][j+4];
                tmp2 = A[_i][j+5];
                tmp3 = A[_i][j+6];
                tmp4 = A[_i][j+7];

                B[j+4][_i] = tmp1;
                B[j+5][_i] = tmp2;
                B[j+6][_i] = tmp3;
                B[j+7][_i] = tmp4;
            }
            
        }
    }
    return;
}

void transpose_64(int M, int N, int A[N][M], int B[M][N]){
    int i, j, _i; //variable for iteration
    int tmp1, tmp2, tmp3, tmp4;
    for(i = 0; i < M; i += 8){
        for(j = 0; j < N; j += 8){
            for(_i = i ; _i < i + 4; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
            }
            for(_i = i + 4; _i < i + 8; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
            }
            for(_i = i ; _i < i + 4; _i++){
                tmp1 = A[_i][j+4];
                tmp2 = A[_i][j+5];
                tmp3 = A[_i][j+6];
                tmp4 = A[_i][j+7];

                B[j+4][_i] = tmp1;
                B[j+5][_i] = tmp2;
                B[j+6][_i] = tmp3;
                B[j+7][_i] = tmp4;
            }
            for(_i = i + 4; _i < i + 8; _i++){
                tmp1 = A[_i][j+4];
                tmp2 = A[_i][j+5];
                tmp3 = A[_i][j+6];
                tmp4 = A[_i][j+7];

                B[j+4][_i] = tmp1;
                B[j+5][_i] = tmp2;
                B[j+6][_i] = tmp3;
                B[j+7][_i] = tmp4;
            }
            
        }
    }
    return;
}

void transpose_61(int M, int N, int A[N][M], int B[M][N]){
    int i, j, _i, _j; //for iteration
    int temp;
    for(i = 0; i < M; i+=4){
        for(j =0; j < N; j+=4){
            for(_i = i; (_i < (i + 4)) && (_i < M); _i++){
                for(_j = j; (_j < (j + 4)) && (_j < N); _j++){
                    temp = A[_j][_i];
                    B[_i][_j] = temp;
                }
            }
        }
    }

}

void transpose_61_1(int M, int N, int A[N][M], int B[M][N]){
    int i, j, _i; //variable for iteration
    int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for(i = 0; i < M; i += 8){
        for(j = 0; j < N; j += 8){
            for(_i = i ; _i < i + 8; _i++){
                tmp1 = A[_i][j];
                tmp2 = A[_i][j+1];
                tmp3 = A[_i][j+2];
                tmp4 = A[_i][j+3];
                tmp5 = A[_i][j+4];
                tmp6 = A[_i][j+5];
                tmp7 = A[_i][j+6];
                tmp8 = A[_i][j+7];

                B[j][_i] = tmp1;
                B[j+1][_i] = tmp2;
                B[j+2][_i] = tmp3;
                B[j+3][_i] = tmp4;
                B[j+4][_i] = tmp5;
                B[j+5][_i] = tmp6;
                B[j+6][_i] = tmp7;
                B[j+7][_i] = tmp8;
            }
        }
    }
    return;
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

