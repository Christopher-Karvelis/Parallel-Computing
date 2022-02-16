/* Serial program for matrix-matrix product.
 *
 * VVD
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1024
int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);

int main(int argc, char *argv[])
{
	double start_total_time, end_total_time, start_execution_time, end_execution_time, overhead_time, total_time, execution_time;
	int i, j, k, sum, WORK, nproc, myid;
	MPI_Status status;
	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	WORK = N/nproc;
	if(myid == 0){
		/* Read matrices from files: "A_file", "B_file" */
		if (readmat("Amat1024", (int *) A, N) < 0) 
			exit( 1 + printf("file problem\n") );
		if (readmat("Bmat1024", (int *) B, N) < 0) 
			exit( 1 + printf("file problem\n") );
		start_total_time = MPI_Wtime();
	}	
	
	int *A_sep = (int *)malloc(WORK*N*sizeof(int));
	int *mypart = (int *)malloc(WORK*N*sizeof(int));


	MPI_Scatter(A, WORK*N, MPI_INT, A_sep, WORK*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	if(myid == 0){
		start_execution_time = MPI_Wtime();
	}
	for (i = 0; i < WORK; i++){
		for (j = 0; j < N; j++){
			for (k = sum = 0; k < N; k++)
				sum += A_sep[i*N+k]*B[k][j];
			mypart[i*N+j] = sum;
		}
	}	
	if(myid == 0){
		end_execution_time = MPI_Wtime();
		execution_time = (end_execution_time - start_execution_time);
	}

	MPI_Gather(mypart, WORK*N, MPI_INT, C, WORK*N, MPI_INT, 0, MPI_COMM_WORLD);

	if(myid == 0){
		end_total_time = MPI_Wtime();
		total_time = (end_total_time - start_total_time);
		overhead_time = (total_time - execution_time);
		/* Save result in "Cmat1024"
		 */
		writemat("Cmat1024", (int *) C, N);
		printf("Total time elapsed = %f seconds\n", total_time);
		printf("Overhead time elapsed = %f seconds\n ", overhead_time);
	}
	MPI_Finalize();
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