#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "mpi.h"



#define AROW 3
#define ACOL 6
 
#define BCOL 4
 
#define MAX_VALUE 12


int main(int argc, char** argv)
{
    int size, rank;
    MPI_Status Stat;
    int row=0;
 int count;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Request *request;
    int a[AROW][ACOL];
    int b[ACOL][BCOL];
   // int c[AROW][BCOL];
   int bT[BCOL][ACOL];
   int c[AROW];
int b1[ACOL];
int output[BCOL][AROW];
int outputT[AROW][BCOL];
int temp =0;
int soutputT[AROW][BCOL];
int sendCounts[10];
int displs[10];
int recvCounts[10];
int recvdispls[10];
double t1,t2,t3,t4;
for(int i=0;i<=BCOL;i++)
{
sendCounts[i]=ACOL;
sendCounts[0]=0;
//printf("Sendcount is %d \n",sendCounts[i]);

displs[i]=(i-1)*ACOL;
displs[0]=0;
//printf("displs is %d \n",displs[i]);



recvCounts[i]=AROW;
recvCounts[0]=0;
//printf("recvCounts is %d \n",recvCounts[i]);

recvdispls[i]=(i-1)*AROW;
recvdispls[0]=0;
//printf("recvdispls is %d \n",recvdispls[i]);

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

//transposing b 

	for(int i=0;i<ACOL;i++)
	{
	    for(int j=0;j<BCOL;j++)
		{

			bT[j][i] =b[i][j];
			
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

//broadcast A matrix to all other process
t1=MPI_Wtime();

MPI_Bcast(a,AROW*ACOL,MPI_INT,0,MPI_COMM_WORLD);


//scatter the B matrix column wise to every process

MPI_Scatterv(bT,sendCounts,displs,MPI_INT,b1,ACOL,MPI_INT,0,MPI_COMM_WORLD);

	

MPI_Gatherv(c,AROW,MPI_INT,output,recvCounts,recvdispls,MPI_INT,0,MPI_COMM_WORLD);

//transposing output matrix

	for(int i=0;i<BCOL;i++)
	{
	    for(int j=0;j<AROW;j++)
		{

			outputT[j][i] =output[i][j];
			
		}

	}

	printf("Output Matrix \n");	
	for(int i=0;i<AROW;i++)
	{
	    for(int j=0;j<BCOL;j++)
		{
			printf("%d \t",outputT[i][j]);
		}
	printf("\n");
	}
	t2=MPI_Wtime();
	printf("The time taken for parallel execution is %f\n",t2-t1);


    }
    else
    {

	MPI_Bcast(a,AROW*ACOL,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Scatterv(bT,sendCounts,displs,MPI_INT,b1,ACOL,MPI_INT,0,MPI_COMM_WORLD);
	


	
	//multiply A with every column of b

	for(int i=0;i<AROW;i++)
	{
	c[i]=0;
	   for(int j=0;j<ACOL;j++)
		{
		c[i]=c[i]+(a[i][j]*b1[j]);
		}
	}


	//send back the multiplied result to root

	MPI_Gatherv(c,AROW,MPI_INT,output,recvCounts,recvdispls,MPI_INT,0,MPI_COMM_WORLD);

	}

   MPI_Finalize();
    return 0;
}
