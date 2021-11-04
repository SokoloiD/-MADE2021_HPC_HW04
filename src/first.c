#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int  selectNextPlayer(int *players_arr, int  total_cnt, int * current_cnt){
	(*current_cnt) ++;
	if (*current_cnt == total_cnt){
		return -1;
	}
	int range = total_cnt - (*current_cnt);
	srand(time(NULL));

	int nextPlayerIndex = *current_cnt + rand() % range;
	int tmp = players_arr[*current_cnt];
	players_arr[*current_cnt] = players_arr[nextPlayerIndex];
	players_arr[nextPlayerIndex] = tmp;
	return players_arr[*current_cnt];


}


int main( int argc, char ** argv){

	
	int psize;
	int prank;
	MPI_Status status;
	int ierr;
	const int  TAG = 1;


	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &prank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &psize);

	int * buffer;

   	buffer = (int*)malloc( (psize + 1) * sizeof(int) );
   	
   	int *players_arr = &buffer[1];
   	int nextPlayerNumber;
 

	if ((prank == 0 )&&(psize > 1)){

		printf(" The number of processes available  is %d\n", psize);
		// init array
		for (int i = 0; i < psize; ++i){
			players_arr[i] = i;
		}

		buffer[0] = 0;
	

	}	

	else{

		MPI_Recv(buffer, psize + 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
	}

	nextPlayerNumber = selectNextPlayer(players_arr, psize, buffer);
	printf("\n from proсess %d  next node %d" , prank, nextPlayerNumber);
	


	if (nextPlayerNumber > 0){

		MPI_Ssend(buffer, psize + 1, MPI_INT, nextPlayerNumber, TAG,  MPI_COMM_WORLD);
		printf("\n Process [%d] finished\n", prank);

	}
	else {
		printf("\nDone\n");
	}


	

	

	free(buffer);
	ierr = MPI_Finalize();

	return 0;
}