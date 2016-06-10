#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Explanation:
 *
 * All matrices are 1D allocated
 *
 * Matrix 1's rows are scattered over the processes.
 * Each process calculates the result of each row in its partition with the entire Matrix 2.
 * The results of each process are gathered into the final array.
 *
 *
 * If there's a remainder of rows, they're sent by MPI_Send from the root to process p-1.
 * Process p-1 computes their result and sends it to the root.
 *
 */


int main(int argc, char *argv[])
{
	int p, my_rank;
	int r1, c1, r2, c2;
	int *mat1, *mat2, *res, chunksize, rem;
	int *mymat, *myres;
	int i,j,k;


	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank==0){

		printf("Enter 1 to read from console\nEnter 2 to read from file\n");
		int choice;
		while(1){
			scanf("%d",&choice);
			if(choice==1 || choice==2)break;
			printf("Enter valid choice\n");
		}

		if(choice==2){
			printf("Enter name of the file to read from\n");
			char name[20];
			scanf("%s",name);
			freopen(name,"r",stdin);
		}



		if(choice==1)printf("Enter dimensions of 1st Matrix.\n");
		scanf("%d %d",&r1,&c1);

		if(choice==1)printf("Enter dimensions of 2nd Matrix.\n");
		scanf("%d %d",&r2,&c2);

		if(c1!=r2){
			printf("Multiplication cannot be done.\n");
			MPI_Finalize();
			return 0;
		}

		mat1=(int*)malloc(r1*c1*sizeof(int));
		if(choice==1)printf("Enter 1st Matrix.\n");
		for(i=0;i<r1*c1;++i)
			scanf("%d",mat1+i);


		mat2=(int*)malloc(r2*c2*sizeof(int));
		if(choice==1)printf("Enter 2nd Matrix.\n");
		for(i=0;i<r2*c2;++i)
			scanf("%d",mat2+i);

		res=(int*)malloc(r1*c2*sizeof(int));


		chunksize = r1/p;
		rem = r1 % p;

		if(rem!=0){
			MPI_Send(mat1+(chunksize*c1*p), rem*c1, MPI_INT, p-1, 0, MPI_COMM_WORLD);
		}

	}

	MPI_Bcast(&r1, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&c1, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&c2, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(my_rank!=0)
		mat2=(int*)malloc(c1*c2*sizeof(int));


	MPI_Bcast(mat2, c1*c2, MPI_INT, 0, MPI_COMM_WORLD);


	chunksize = r1/p;
	rem = r1 % p;


	if(my_rank==p-1)
		mymat=(int*)malloc((chunksize+rem)*c1*sizeof(int));

	else
		mymat=(int*)malloc(chunksize*c1*sizeof(int));



	MPI_Scatter(mat1, chunksize*c1, MPI_INT, mymat, chunksize*c1, MPI_INT, 0, MPI_COMM_WORLD);


	if(my_rank==p-1 && rem!=0){
		MPI_Recv(mymat+(chunksize*c1), rem*c1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		chunksize+=rem;
	}


	myres=(int*)malloc(chunksize*c2*sizeof(int));

	int ind=0;
	for(i=0;i<chunksize;++i){
		for(j=0;j<c2;++j){
			myres[ind]=0;
			for(k=0;k<c1;++k){
				myres[ind]+=(mymat[(i*c1)+k]*mat2[(k*c2)+j]);
			}
			++ind;
		}
	}

	if(my_rank==p-1 && rem!=0){
		chunksize-=rem;
		MPI_Send(myres+(chunksize*c2), rem*c2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Gather(myres, chunksize*c2, MPI_INT, res, chunksize*c2, MPI_INT, 0, MPI_COMM_WORLD);


	if(my_rank==0){
		if(rem!=0)
			MPI_Recv(res+(chunksize*c2*p), rem*c2, MPI_INT, p-1, 1, MPI_COMM_WORLD, &status);

		ind=0;
		printf("The resulting Matrix:\n");
		for(i=0;i<r1;++i){
			for(j=0;j<c2;++j){
				printf("%d ",res[ind++]);
			}
			printf("\n");
		}
	}


	MPI_Finalize();

	return 0;
}
