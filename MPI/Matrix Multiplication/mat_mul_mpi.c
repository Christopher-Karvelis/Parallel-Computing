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
		for (i = 1; i < nproc; i++) { 
			MPI_Send(A[i*WORK], WORK*N, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(B, N*N, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		for (i = 0; i < WORK; i++){
			for (j = 0; j < N; j++)
			{
				for (k = sum = 0; k < N; k++)
					sum += A[i][k]*B[k][j];
				C[i][j] = sum;
			};
		}
		int *mypart = (int *)malloc(WORK*N*sizeof(int));
		for (i = 1; i < nproc; i++) {
			MPI_Recv(mypart, WORK*N, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			for (j = 0; j < WORK; j++){ /* Place elements */
				for(k = 0; k < N; k++){
					C[j + status.MPI_TAG][k] = mypart[j*N+k];
				}	
			}
				
		}	
		
		/* Save result in "Cmat1024"
		 */
		writemat("Cmat1024", (int *) C, N);
	}else{		
								/* All other processes */
		int *A = (int *)malloc(WORK*N*sizeof(int));
		int *mypart = (int *)malloc(WORK*N*sizeof(int));

		MPI_Recv(A, WORK*N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(B, N*N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		for (i = 0; i < WORK; i++){
			for (j = 0; j < N; j++){
				for (k = sum = 0; k < N; k++)
					sum += A[i*N+k]*B[k][j];
				mypart[i*N+j] = sum;
			}
		}	
		MPI_Send(mypart, WORK*N, MPI_INT, 0, myid*WORK, MPI_COMM_WORLD);
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