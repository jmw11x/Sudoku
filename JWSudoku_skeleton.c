/**
 * A skeleton for checking a Sudoku puzzle through multithread

 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define NUMBER_OF_THREADS 	21 // we use 9 threads in this project
#define PUZZLE_SIZE 		9  // it is a 9x9 puzzle


/* example puzzle */

// this is a sample table, the values will be replaced by those read from the input file, except -1
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
			{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
			{-1,1,3,4,6,7,8,9,1,2},
			{-1,6,7,2,1,9,5,3,4,8},
			{-1,1,9,8,3,4,2,5,6,7},
			{-1,8,5,9,7,6,1,4,2,3},
			{-1,4,2,6,8,5,3,7,9,1},
			{-1,7,1,3,9,2,4,8,5,6},
			{-1,9,6,1,5,3,7,2,8,4},
			{-1,2,8,7,4,1,9,6,3,5},
			{-1,3,4,5,2,8,6,1,7,9}
		};


int status_map[NUMBER_OF_THREADS] = {0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0};

/* data structure for passing data to threads */
typedef struct
{
	int thread_number;
	int i;
	int j;
	int mapIndex;
} parameters;

void *rowCheck(void *param){
	parameters *params = param;
	int i, row = params->i;
//simplified version of a hashmap to keep track of visited numbers.
//reused same concept in functions for column and grid
	int map[] = {0,0,0,0,0,0,0,0,0};
	for(i=1;i<10;i++){
		map[puzzle[row][i]-1]++;
	}
	for(i=0;i<9;i++){
		if(map[i]!=1){
			printf("NOT VALID r tid: %d\n", params->thread_number);
			pthread_exit(0);
			//set to false in shared array (is by default, here i use 0 as false)
		}
	}
	status_map[params->mapIndex] = 1;
	pthread_exit(0);
	free(param);
}
void *columnCheck(void *param){
	parameters *params=param;

	int i, col = params->j;
	int map[] = {0,0,0,0,0,0,0,0,0};
	for(i=1;i<10;i++){
		map[puzzle[i][col]-1]++;
	}
	for(i=0;i<9;i++){
		if(map[i]!=1){
			printf("NOT VALID c tid: %d\n", params->thread_number);
			pthread_exit(0);
		}
	}
	status_map[params->mapIndex] = 1;
	pthread_exit(0);
	free(param);
}
//checks one 9*3 area (3 subgrids independently)
void *checkbox(void *param){
	parameters *params=param;
	int map[] ={0,0,0,0,0,0,0,0,0};
	int row = params->i;
	int column = params->j;
	int i, j, count = 1,k;
	for(i=row;i<10;i++){
		for(j=column;j<column+3;j++){
			map[puzzle[i][j]-1]++;
		}
		if(count%3==0){

			for(k=0;k<9;k++){
				if(map[k]==0){
					printf("Invalid thread tid: %d\n", params->thread_number);
					pthread_exit(0);
				}
				map[k]=0;
			}
		}
		count++;
	}
	status_map[params->mapIndex] = 1;
	pthread_exit(0);
	free(param);
}
void validSudoku(){
	int i;
	for(i=0;i<NUMBER_OF_THREADS;i++){
		if(status_map[i]==0){
			printf("Invalid SUDOKU SEE THREAD NUMBER FOR INVALID\n");
			return;
		}
	}
	printf("Valid Sudoku, all %d threads were correct\n", NUMBER_OF_THREADS);
}

int main(int argc, char *argv[])
{
	parameters *Parameters = (parameters*)malloc(sizeof(parameters));
	// get a puzzle from a txt file, which needs to be input from command line
	char filename[100];
	printf("Please enter your filename:\n");
	scanf("%s",filename);
	Parameters->mapIndex = 0;

  FILE *myFile;
  myFile = fopen(filename, "r");

   //read file into array
   int i,j;

   if (myFile == NULL)
   {
       printf("Error Reading File\n");
       exit (0);
   }
   for (i = 1; i < PUZZLE_SIZE + 1; i++)
   {
		for (j =1; j < PUZZLE_SIZE + 1; j++)
		{
			fscanf(myFile, "%d,", &puzzle[i][j] );
		}

   }
	pthread_t tid;//identifier
	pthread_attr_t attr;;//set of attributes
	pthread_attr_init(&attr);
	pthread_t workers[NUMBER_OF_THREADS];

/*
	I create 3 for loops. They create a new thread in each iteration of
	the individual loops, wait for them to complete and repeat.
	  => 9 threads for column and row check functions, 3 for grid.
	TODO: Refactor into 3 seperate functions if enough time.
*/
	Parameters->i=1;
	Parameters->j =-1;
	Parameters->thread_number=tid;
	for(i=1;i<10;i++){
			pthread_create(&tid,&attr, rowCheck, Parameters);
			pthread_join(tid,NULL);//let thread finish before creating new
			Parameters->i = i+1;
			Parameters->j =-1;
			Parameters->thread_number=tid;
			Parameters->mapIndex++;
	}
	Parameters->i = -1;
	Parameters->j=1;
//create the 9 threads for column check
	for(i=1;i<10;i++){
		pthread_create(&tid,&attr, columnCheck, Parameters);
		pthread_join(tid,NULL);
		Parameters->i=-1;
		Parameters->j=i+1;
		Parameters->thread_number=tid;
		Parameters->mapIndex++;
	}
//create the 3 threads for the 3 9x3 grids
	for(int i =1; i < 10; i+=3){
		Parameters->i=1;
		Parameters->j=i;
		Parameters->thread_number=tid;
		pthread_create(&tid,&attr,checkbox, Parameters);
		pthread_join(tid,NULL);
		Parameters->mapIndex++;
	}
	validSudoku();//checks global map of valid or invlaid threads prints message
	free(Parameters);
	return 0;
}
