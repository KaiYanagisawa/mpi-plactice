#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAXN   100
unsigned char  addrs[MAXN][4];
short          ports[MAXN];

int main(int argc, char *argv[])
{
  int rank;
  MPI_Status status;

  unsigned int membersize, maxsize;
  int position;
  int nhosts;
  char *buffer;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Pack_size(1,MPI_INT,MPI_COMM_WORLD, &membersize);
  maxsize=membersize;
  MPI_Pack_size(MAXN*4,MPI_UNSIGNED_CHAR,MPI_COMM_WORLD, &membersize);
  maxsize+=membersize;
  MPI_Pack_size(MAXN,MPI_SHORT,MPI_COMM_WORLD, &membersize);
  maxsize+=membersize;
  buffer=(void *)malloc(maxsize);

  if(rank==0){
    nhosts=20;
    position=0;
    for(int i=0; i<nhosts; i++){
      addrs[i][0]=133; addrs[i][1]=220; addrs[i][2]=114;
      addrs[i][3]=i+1; ports[i]=rand()%750+250;
    }
    MPI_Pack(&nhosts,1,MPI_INT,buffer,maxsize,&position,MPI_COMM_WORLD);
    MPI_Pack(addrs,nhosts*4,MPI_UNSIGNED_CHAR,buffer,maxsize,&position,MPI_COMM_WORLD);
    MPI_Pack(ports,nhosts,MPI_SHORT,buffer,maxsize,&position,MPI_COMM_WORLD);
    MPI_Send(buffer,position,MPI_PACKED,1,111,MPI_COMM_WORLD);
  }else{
    int msgsize;
    MPI_Recv(buffer,maxsize,MPI_PACKED,0,111,MPI_COMM_WORLD,&status);
    position=0;
    MPI_Get_count(&status,MPI_PACKED,&msgsize);
    MPI_Unpack(buffer,msgsize,&position,&nhosts,1,MPI_INT,MPI_COMM_WORLD);
    MPI_Unpack(buffer,msgsize,&position,addrs,nhosts*4,MPI_UNSIGNED_CHAR,MPI_COMM_WORLD);
    MPI_Unpack(buffer,msgsize,&position,ports,nhosts,MPI_SHORT,MPI_COMM_WORLD);

    printf("Number of hosts = %d\n", nhosts);
    printf("Hosts address -----\n");
    for(int i=0; i<nhosts; i++){
      for(int j=0; j<4; j++) printf("%03d%c",addrs[i][j],(j<3)?'.':':');
      printf("%3d\n",ports[i]);
    }
  }
  MPI_Finalize();
  return 0;
}


