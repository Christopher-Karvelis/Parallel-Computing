#include <stdio.h>
#include <omp.h>
#include <mpi.h>

#define UPTO 10000000

long int count,      /* number of primes */
         lastprime;  /* the last prime found */


void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;  
			remainder = num % divisor;  
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			count++;
			lastprime = num;
		}
	}
}


void mpi_primes(long int n) {
	double start_total_time, end_total_time, start_execution_time, end_execution_time, overhead_time, total_time, execution_time;
	long int i, num, divisor, quotient, remainder, lastprime_0, count_0;
	long int temp_count,      /* number of primes */
         		temp_lastprime;  /* the last prime found */

	int myid, nproc, WORK;
	MPI_Status status;
	
	if (n < 2) return;
	lastprime = 2;                     /* 2 is the first prime */
	count = 1;
  
	/* 
	 * Parallelize the serial algorithm but you are NOT allowed to change it!
	 * Don't add/remove/change global variables
	 */

	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	if(myid == 0){
		start_total_time = MPI_Wtime();
	}
	if(myid == 0){
		start_execution_time = MPI_Wtime();
	}
	for (i = myid; i < (n-1)/2; i+= nproc){ /* Doing my own work */

		num = 2*i + 3;

		divisor = 1;
		do 
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;  
			remainder = num % divisor;  
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			count++;
			lastprime = num;
		}
	}
	if(myid == 0){
		end_execution_time = MPI_Wtime();
		execution_time = (end_execution_time - start_execution_time);
	}

	/*Gather all results*/
	MPI_Reduce(&lastprime, &lastprime_0, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&count, &count_0, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

	if(myid == 0){
		count_0 -= (nproc - 1);
		end_total_time = MPI_Wtime();
		total_time = (end_total_time - start_total_time);
		overhead_time = (total_time - execution_time);
		printf("[mpi] count = %ld, last = %ld (Total time = %f, Overhead time = %f)\n", count_0, lastprime_0, total_time, overhead_time);
	}
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	//printf("Serial and parallel prime number calculations:\n\n");
	/* Time the following to compare performance 
	 */
	//serial_primes(UPTO);        /* time it */
	//printf("[serial] count = %ld, last = %ld (time = ...)\n", count, lastprime);
	mpi_primes(UPTO);        /* time it */
	MPI_Finalize();
	return 0;
}