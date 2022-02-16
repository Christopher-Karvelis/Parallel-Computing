/* Program with tasks for matrix-matrix product.
 *
 * VVD
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1024
int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);
void checker(int M, int S, int id);

int main(int argc, char *argv[])
{
	int S = atoi(argv[1]);
	int M = N/S;
	int num_of_sub_arrays = M*M;
	double start; 
	double end; 

	/* Read matrices from files: "A_file", "B_file" 
	 */
	if (readmat("Amat1024", (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat("Bmat1024", (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );

	start = omp_get_wtime(); 
	#pragma omp parallel
	{
		#pragma omp single
		{
			for (int i = 0; i < num_of_sub_arrays; ++i)
			{
				#pragma omp task firstprivate(i)
					checker(M, S, i);
			}		
		}
	}
	end = omp_get_wtime(); 
	printf("Work took %f seconds\n", end - start);
	/* Save result in "Cmat1024"
	 */
	writemat("Cmat1024", (int *) C, N);

	return (0);
}

/* Utilities to read & write matrices from/to files
 * VVD
 */

#define _mat(i,j) (mat[(i)*n + (j)])

int readmat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "r")) == NULL)
		return (-1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (fscanf(fp, "%d", &_mat(i,j)) == EOF)
			{
				fclose(fp);
				return (-1); 
			};
	fclose(fp);
	return (0);
}

int writemat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "w")) == NULL)
		return (-1);
	for (i = 0; i < n; i++, fprintf(fp, "\n"))
		for (j = 0; j < n; j++)
			fprintf(fp, " %d", _mat(i, j));
	fclose(fp);
	return (0);
}

void checker(int M, int S, int id) {
	int i, j, k, x, y, sum;
	int	me = id;
	x = me / M;
	y = me % M;
	for (i = x*S; i < (x+1)*S; i++) { /* calculate Cxy */
		for (j = y*S; j < (y+1)*S; j++) {
			for (k = 0, sum = 0; k < N; k++)
				sum += A[i][k]*B[k][j];
			C[i][j] = sum;
		}
	}
}
