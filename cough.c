#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


//Mackenzie Colwell

int main (int argc, char* argv[]) {

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

  //Start up MPI
  MPI_Init(&argc, &argv);

  //Find this process's number
  MPI_Comm_rank(MPI_COMM_WORLD, &this_p);

  //Find number of processes total
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  
  if (this_p == p - 1) {
    printf("hello world");
    int size = sqrt(p);
      sprintf(message, "north", size);
      for (destination = 0; destination < size; destination++) {
        MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
      }
      sprintf(message, "south", size);
      for (destination = this_p; destination > destination - size; destination--) {
        MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
      }
      sprintf(message, "east", size);
      for (destination = size - 1; destination <= this_p; destination = destination + size) {
        MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
      }
      sprintf(message, "west", size);
      for (destination = 0; destination < this_p; destination = destination + size) {
        MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
      }
  }

  //This while loop continues to recieve messages until no message is an edge declaration. Each time it recieves an edge declaration, it sets the edge value in the relevant part of edges[]
  char done = 'n';
  while (done != 'y') {
    MPI_Recv(message, 10, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    if (strcmp(message,"north") == 0) {
      edges[0] = 1;
    }
    else if (strcmp(message,"east") == 0) {
      edges[1] = 1;
    }
    else if (strcmp(message,"west") == 0) {
      edges[2] = 1;
    }
    else if (strcmp(message,"south") == 0) {
      edges[3] = 1;
    }
    else {
      done = 'y';
    }
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
    strcat("e",message);
  }
    
  //strlen+1 to get null termination of string
	//Send initial message to process 0 to indicate health status
  MPI_Send(message, strlen(message)+1, MPI_CHAR, destination, tag, MPI_COMM_WORLD);
  
  //Read all initial illness status messages and print map
  if (this_p == 0) {
    source = 0;
    while (source < p) {
      MPI_Recv(message, 10, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      if (strcmp(message,"cough") == 0 || strcmp(message,"ecough") == 0) {
        printf("x");
      }
      else {
        printf(" ");
      }
      if (strcmp(message,"ecough") == 0 || strcmp(message,"e:)") == 0) {
        printf("\n");
      }
      source++;
    }
  }

  MPI_Finalize();
  return 0;
}
