#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "mpi.h"
#include <omp.h>


#define AROW 10
#define ACOL 10
 
#define BCOL 10
 
#define MAX_VALUE 10



int main(int argc, char** argv)
{
    int size, rank;
    MPI_Status Stat;
    int row=0;
	
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    

	int a[AROW][ACOL];
	int b[ACOL][BCOL];
	int c[AROW][BCOL];

	int output[AROW][BCOL];


	int sendCounts[size];
	int displs[size];
	int recvCounts[size];
	int recvdispls[size];
	int i,j,k;

	double t1,t2,t3,t4;

	int numberofworkers = size-1;	
	int averagerows = AROW/numberofworkers;
	int extra = AROW%numberofworkers;
	int rows=0;
	int a1[averagerows+1][ACOL];	

	for(int i=1;i<=numberofworkers;i++)
	{

	 rows= (i<=extra)?averagerows+1:averagerows;
 	 sendCounts[0]=0;
	 sendCounts[i]=rows*ACOL;
	

	 displs[0]=0;
	 displs[i]=sendCounts[i-1]+displs[i-1];

	 recvCounts[0]=0;
	 recvCounts[i]=rows*BCOL;
	 
	 recvdispls[0]=0;
	 recvdispls[i]=recvCounts[i-1]+recvdispls[i-1];
	 


	}


    if(rank==0)
    {
	

    	// generating random values for matrix A and B
	
	for(int i=0;i<AROW;i++)
	{
	    for(int j=0;j<ACOL;j++)
		{
		 a[i][j] = rand() % MAX_VALUE;
		}

	}

	for(int i=0;i<ACOL;i++)
	{
	    for(int j=0;j<BCOL;j++)
		{
		 b[i][j] = (rand() % MAX_VALUE);
		}

	}

	//printing the values A and B
	printf("Matrix A \n");
	
	for(int i=0;i<AROW;i++)
	{
	    for(int j=0;j<ACOL;j++)
		{
			printf("%d \t",a[i][j]);
		}
	printf("\n");

	}
	
	printf("Matrix B \n");
	for(int i=0;i<ACOL;i++)
	{
	    for(int j=0;j<BCOL;j++)
		{
			printf("%d \t",b[i][j]);
		}
	printf("\n");
	}

	//broadcast B matrix to all other process
	t1=MPI_Wtime();

	MPI_Bcast(b,ACOL*BCOL,MPI_INT,0,MPI_COMM_WORLD);


	//scatter the A matrix Row wise wise to every process

	MPI_Scatterv(a,sendCounts,displs,MPI_INT,a1,sendCounts[rank],MPI_INT,0,MPI_COMM_WORLD);


	//gather the output from the workers
	MPI_Gatherv(c,recvCounts[rank],MPI_INT,output,recvCounts,recvdispls,MPI_INT,0,MPI_COMM_WORLD);

	t2=MPI_Wtime();

	printf("Output Matrix \n");
	
	for(int i=0;i<AROW;i++)
	{
	    for(int j=0;j<BCOL;j++)
	    {
		printf("%d \t",output[i][j]);
	    }
	    
          printf("\n");
	}


	printf("The time taken for parallel execution is %f\n",t2-t1);
	
    }
    else
    {
	//recieve the whole matrix B
	
	MPI_Bcast(b,ACOL*BCOL,MPI_INT,0,MPI_COMM_WORLD);

	//Recieve Rows of A

	MPI_Scatterv(a,sendCounts,displs,MPI_INT,a1,sendCounts[rank],MPI_INT,0,MPI_COMM_WORLD);
	
	//multiply every row of  A with  b
	//#pragma omp parallel for shared(a1,b,c) private(i,j,k) schedule(static,2)
	for(int i=0;i<sendCounts[rank]/ACOL;i++)
	{
	   for(int j=0;j<BCOL;j++)
	   {
		c[i][j]=0;
		for(int k=0;k<ACOL;k++)
		{
		  c[i][j]=c[i][j]+(a1[i][k]*b[k][j]);
		}
	   }
	}
	

	//send back the multiplied result to root

	MPI_Gatherv(c,recvCounts[rank],MPI_INT,output,recvCounts,recvdispls,MPI_INT,0,MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}
