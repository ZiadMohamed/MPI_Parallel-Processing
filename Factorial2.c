#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER 0


int main(int argc, char *argv[])
{
	int p, my_rank;
	int n, r, i;
	long long prod=1, res=1;
	double strt;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank==MASTER){
		printf("Hello from master process.\nNumber of slave processes is %d\nPlease enter N...\n",p-1);
		scanf("%d",&n);
		strt = MPI_Wtime();

		if(p==1){
			for(i=1;i<=n;++i)
				res*=i;

			printf("After reduction, P0 will have factorial(%d) = %lld\nExecution time = %lf seconds\n",n,res,MPI_Wtime()-strt);

			MPI_Finalize();
			return 0;
		}

		r=n/(p-1);

	}

	MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&r, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

	if(my_rank!=MASTER){

		int a=(r*(my_rank-1))+1;
		int b=a+r-1;
		if(my_rank==p-1)b=n;


		prod=1;
	    for(i=a;i<=b;++i)
	    	prod*=i;

	    if(a<=b)
	    	printf("p%d: calculate partial factorial from %d to %d = %lld\n",my_rank,a,b,prod);
	    else
	    	printf("p%d: empty\n",my_rank);


	}

	MPI_Reduce(&prod, &res, 1, MPI_LONG_LONG, MPI_PROD, MASTER, MPI_COMM_WORLD);

	if(my_rank==MASTER)
		printf("After reduction, P0 will have factorial(%d) = %lld\nExecution time = %lf seconds\n",n,res,MPI_Wtime()-strt);


	MPI_Finalize();

	return 0;
}
