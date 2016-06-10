#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER 0


int main(int argc, char *argv[])
{
	int p, my_rank, source, dest;
	int tag1, tag2, tag3;
	int n, r, i;
	long long prod, res;
	double strt;

	tag1=1;
	tag2=2;
	tag3=3;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank==MASTER){
		printf("Hello from master process.\nNumber of slave processes is %d\nPlease enter N...\n",p-1);
		scanf("%d",&n);
		strt = MPI_Wtime();

		if(p==1){
			res=1;
			for(i=1;i<=n;++i)
				res*=i;

			printf("After reduction, P0 will have factorial(%d) = %lld\nExecution time = %lf seconds\n",n,res,MPI_Wtime()-strt);

			MPI_Finalize();
			return 0;
		}



		r=n/(p-1);
		for(dest=1;dest<p;++dest){
			MPI_Send(&n, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
			MPI_Send(&r, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
		}



		res=1;
		for(source=1;source<p;++source){
			MPI_Recv(&prod, 1, MPI_LONG_LONG, source, tag3, MPI_COMM_WORLD, &status);
			res*=prod;
		}

		printf("After reduction, P0 will have factorial(%d) = %lld\nExecution time = %lf seconds\n",n,res,MPI_Wtime()-strt);
	}
	else{
		source=MASTER;
		MPI_Recv(&n, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		MPI_Recv(&r, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);


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

	    dest=MASTER;
	    MPI_Send(&prod, 1, MPI_LONG_LONG, dest, tag3, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
