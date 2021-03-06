#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


//Mackenzie Colwell

int main (int argc, char* argv[]) {

  //Start up MPI
  MPI_Init(&argc, &argv);

  int this_p; //which process this is
  int p; //total number of processes
  int source; //process number of sender
  int destination; //process number of receiver
  int tag = 0; //tag for messages
  char message[10]; //storage for message
  MPI_Status status; //status of return

  //The edges keeps track of the 'edges' of the map. The map is a traditional grid and coughing cannot spread beyond the map or originate from outside the map. If an edge is set to true, then coughing that attempts to spread in that direction will not be effective.
  char edges[3];
  int counter = 0;
  while (counter < 4) {
    edges[counter] = 0;
    counter++;
  }

  //Find this process's number
  MPI_Comm_rank(MPI_COMM_WORLD, &this_p);
  //Find number of processes total
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  
  printf("hi from:%d of %d\n", this_p,p);
  

  int width = sqrt(p);
  
  //north
  if (this_p < width) {
    edges[0] = 1;
  }

  //south
  if (this_p >= p - width) {
    edges[2] = 1;
  }
  
  //west
  if (this_p % width == 0) {
    edges[3] = 1;
  }

  //east
  if (this_p % width == width - 1) {
    edges[1] = 1;
  }

  //next two lines set up to get us a random number
  time_t t;
  srand((unsigned) time(&t));

  //Create initial cough message to process 0 IFF randomly selected to be ill, otherwise send healthy message
  if (rand()%5 == 0) {
    sprintf(message, "cough");
  }
  else {
    sprintf(message, ":)");
  }
  destination = 0;
  //add e to beginning of message if it is an east edge to signal that the map should print a newline after it
  if (edges[1] == 1) {
    tag = 1;
  }
    
  //strlen+1 to get null termination of string
  //Send initial message to process 0 to indicate health status
  MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
  printf("sent message from %d to %d\n", this_p, destination);
  //Read all initial illness status messages and print map
  if (this_p == 0) {
    printf("initial map\n");
    source = 0;
    while (source < p) {
      MPI_Recv(message, 10, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      printf("Recieved message from %d ... %s\n", source,message);
      if (strncmp(message,"cough",6) == 0) {
        printf("x");
      }
      else {
        printf("-");
      }
      if (tag == 1) {
        printf("\n");
      }
      source++;
    }
  }
  printf("Done [%d]\n",this_p);

  MPI_Finalize();
  return 0;
}
