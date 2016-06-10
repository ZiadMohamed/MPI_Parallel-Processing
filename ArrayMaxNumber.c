#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER 0


int main(int argc, char *argv[])
{
	int numtasks, taskid, source, dest;
	int tag1, tag2, tag3, tag4;
	int totalSz, chunksize, offset;
	int curmax, curmaxIndex, finalMax, finalMaxIndex;
	int i;

	tag1=1;
	tag2=2;
	tag3=3;
	tag4=4;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

	if(taskid==MASTER){
		printf("Hello from master process.\nNumber of slave processes is %d\nPlease enter size of array...\n",numtasks-1);


		scanf("%d",&totalSz);

		int *arr=(int*)malloc(totalSz*sizeof(int));

		puts("Please enter array elements ...");
		for(i=0;i<totalSz;++i)
			scanf("%d",arr+i);

		if(numtasks==1){
			finalMaxIndex=-1;
			for(i=0;i<totalSz;++i){
				if(finalMaxIndex==-1 || arr[i]>finalMax)
					finalMax=arr[i], finalMaxIndex=i;
			}
			printf("\nMaster process announce the final max which is %d and its index is %d.\nThanks for using our program\n",finalMax,finalMaxIndex);

			MPI_Finalize();
			return 0;
		}

		int portion = totalSz / (numtasks-1);
		int rem = totalSz % (numtasks-1);

		offset=0;
		for(dest=1;dest<numtasks;++dest){
			chunksize = portion + (dest-1<rem);

			MPI_Send(&chunksize, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);

			if(chunksize>0)
				MPI_Send(arr+offset, chunksize, MPI_INT, dest, tag2, MPI_COMM_WORLD);

			offset+=chunksize;
		}

		finalMaxIndex=-1;
		offset=0;
		for(source=1;source<numtasks;++source){
			chunksize = portion + (source-1<rem);

			MPI_Recv(&curmax, 1, MPI_INT, source, tag3, MPI_COMM_WORLD, &status);
			MPI_Recv(&curmaxIndex, 1, MPI_INT, source, tag4, MPI_COMM_WORLD, &status);

			if(curmaxIndex==-1)break;

			if(finalMaxIndex==-1 || curmax>finalMax)
				finalMax=curmax, finalMaxIndex= offset + curmaxIndex;

			offset+=chunksize;
		}

		printf("\nMaster process announce the final max which is %d and its index is %d.\nThanks for using our program\n",finalMax,finalMaxIndex);

	}
	else{
		source=MASTER;
		MPI_Recv(&chunksize, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);

		int *arr=(int*)malloc(chunksize*sizeof(int));

	    if(chunksize>0)
	    	MPI_Recv(arr, chunksize, MPI_INT, source, tag2,MPI_COMM_WORLD, &status);

	    curmaxIndex=-1;
	    for(i=0;i<chunksize;++i){
	    	if(curmaxIndex==-1 || arr[i]>curmax)
	    		curmax=arr[i], curmaxIndex=i;
	    }


	    if(curmaxIndex==-1)printf("Hello from slave#%d My partition is empty\n",taskid);
	    else printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n",taskid,curmax,curmaxIndex);

	    dest=MASTER;
	    MPI_Send(&curmax, 1, MPI_INT, dest, tag3, MPI_COMM_WORLD);
	    MPI_Send(&curmaxIndex, 1, MPI_INT, dest, tag4, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
