#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void  initArray(int *arr, int  size){
	srand(time(NULL));
	for (int i = 1; i <= size; ++i){
		arr[i] = rand() % 2;
	}

}


void initRulesArr(int *rulesArr, int rule){

	for(int i = 0; i < 8; ++i){
		rulesArr[i] = rule & 1;
		rule >>= 1;
	
	}
}


void playTheGame(int *sourceArr, int * tmpArr, int size, int *rules){
	int ruleNum;

	for(int i =1; i<= size; ++i){
		ruleNum = (sourceArr[i - 1] << 2) + 
				  (sourceArr[i] << 1) + 
				  sourceArr[i + 1] ;
		//printf("r%d", ruleNum);
		tmpArr[i] = rules[ruleNum];
	}

	for(int i =1; i<= size; ++i){
		sourceArr[i] = tmpArr[i];
	}

}

void printArray( int * buffer, int size){
	
	for(int i = 1; i <= size+1; ++i){
		if(buffer[i]){
			printf("*");
		}
		else{
			printf("_");
		}

		
	}
}
int main( int argc, char ** argv){

	
	const int  TAG = 1;
	const int FIELD_SIZE = 20;
	const int BUFFER_SIZE = FIELD_SIZE + 2;
	const int ITERATION_NUM = 40;

	MPI_Status status;
	int psize;
	int prank;
	int ierr;
	int rule;
	int rulesArr[8];


	if(argc ==2){

		rule = atoi(argv[1]);

	} else {

		rule = 110;
	}
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &prank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &psize);

	int * buffer, *tmpBuffer;

   	buffer = (int*)malloc( BUFFER_SIZE * sizeof(int) );
   	tmpBuffer = (int*)malloc( BUFFER_SIZE * sizeof(int) );


   	//circle topology 

   	int leftNeighbour = prank - 1;
   	if (leftNeighbour == -1){
   		leftNeighbour = psize - 1;
   	}

   	int rightNeighbour = prank + 1;
   	if (rightNeighbour == psize){
   		rightNeighbour = 0;
   	}
 

	initArray(buffer, FIELD_SIZE);
	initRulesArr(rulesArr, rule);
	

	for(int i = 0; i < ITERATION_NUM; ++i){
		// update ghost cells	

		// send  ghost cells
		MPI_Send(&buffer[1], 1, MPI_INT, leftNeighbour, TAG,  MPI_COMM_WORLD);			
		MPI_Send(&buffer[FIELD_SIZE], 1, MPI_INT, rightNeighbour, TAG,  MPI_COMM_WORLD);	

		// receive left and right ghost cells		
		MPI_Recv(&buffer[0], 1, MPI_INT, leftNeighbour, TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&buffer[FIELD_SIZE + 1],1, MPI_INT, rightNeighbour, TAG, MPI_COMM_WORLD, &status);

		playTheGame(buffer, tmpBuffer, FIELD_SIZE, rulesArr);

		if(prank == 0){
			
			printf("\n");
			printArray(buffer, FIELD_SIZE);
			for(int proc_num = 1; proc_num < psize; ++ proc_num){
				MPI_Recv(&tmpBuffer[1],FIELD_SIZE, MPI_INT, proc_num, TAG, MPI_COMM_WORLD, &status);
				printArray(tmpBuffer, FIELD_SIZE);


			}

		} else {
			MPI_Send(&buffer[1], FIELD_SIZE, MPI_INT, 0, TAG,  MPI_COMM_WORLD);		

		}


	}


	free(buffer);
	free(tmpBuffer);

	ierr = MPI_Finalize();

	return 0;
}