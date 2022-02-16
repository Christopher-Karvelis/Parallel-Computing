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
	long int i, num, divisor, quotient, remainder;
	long int temp_count,      /* number of primes */
         		temp_lastprime;  /* the last prime found */
	int myid, nproc, WORK;
	MPI_Status status;
	
	
	if (n < 2) return;
	lastprime = 2;                     /* 2 is the first prime */
	
  
	/* 
	 * Parallelize the serial algorithm but you are NOT allowed to change it!
	 * Don't add/remove/change global variables
	 */

	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	WORK = ((n-1)/2)/nproc;

	if(myid == 0){ 				/* Proccess 0 */
		count = 1;  

		for (i = 0; i < WORK; i++){ /* Doing my own work */

			num = 2*(myid*WORK+i) + 3;

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
		

		/*Gather all results*/
		for (i = 1; i < nproc; i++){
			MPI_Recv(&temp_lastprime, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (temp_lastprime > lastprime){
				lastprime = temp_lastprime;
			}
			MPI_Recv(&temp_count, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			count += temp_count;
		}
		//printf("HI");
		printf("[mpi] count = %ld, last = %ld (time = ...)\n", count, lastprime);
	}
	else{							/* All other proccesses */
		count = 0;
		for (i = 0; i < WORK; i++){

			num = 2*(myid*WORK+i) + 3;

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
		printf("myid = %d", myid);
		MPI_Send(&lastprime, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&count, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
	}
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	//printf("Serial and parallel prime number calculations:\n\n");
	/* Time the following to compare performance 
	 */
	serial_primes(UPTO);        /* time it */
	printf("[serial] count = %ld, last = %ld (time = ...)\n", count, lastprime);
	mpi_primes(UPTO);        /* time it */
	MPI_Finalize();
	return 0;
}