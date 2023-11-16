#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#define COL 6
#define ROW 9

int main(int argc, char *argv[])
{
  int size, rank;
  int a[COL][ROW]={0};
  int i, j;
  MPI_Status status;
  MPI_Datatype mytype;
  int stc, str;
  int height,width;

  if(argc!=5){
    printf("Please give start(col,row) and block(height,width).");
    exit(1);
  }
  stc=atoi(argv[1]);  str=atoi(argv[2]);
  height=atoi(argv[3]); width=atoi(argv[4]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Type_vector(height,width,ROW,MPI_INT,&mytype);
  MPI_Type_commit(&mytype);

  if(rank==0){
    for(i=0; i<COL; i++)
      for(j=0; j<ROW; j++) a[i][j]=(i+1)*10+j;
    MPI_Send(&a[stc][str],1,mytype,1,10,MPI_COMM_WORLD);
  }else{
    int count;
    /*    for(i=0; i<COL; i++)
	  for(j=0; j<ROW; j++) a[i][j]=0;*/
    MPI_Recv(&a[stc][str],1,mytype,0,10,MPI_COMM_WORLD,&status);
    MPI_Get_count(&status, MPI_INT, &count);  // mytypeでは１個、MPI_INTではheight*width個となる
    printf("Rank1 get %d data.\n",count);
    printf("Rank1 has following values.....\n");
    for(i=0; i<COL; i++){
      for(j=0; j<ROW; j++) printf("%3d",a[i][j]);
      printf("\n");
    }
  }

  MPI_Type_free(&mytype);
  MPI_Finalize();
  return 0;
}
