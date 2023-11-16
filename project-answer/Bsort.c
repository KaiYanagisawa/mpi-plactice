#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/time.h>

double gettime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return((double)(tv.tv_sec)+(double)(tv.tv_usec)*1e-6);
}

//#define NUM 50000
//#define MAX 50000

#define NRow 15

int main(int argc, char *argv[])
{
  int rank, size;
  int *data,sum=0;
  double stime,etime;
  int MAX=atoi(argv[1]), NUM=atoi(argv[2]);
  data=(int*)malloc(sizeof(int)*NUM); 

  printf("----- Before sort -----\n");
  for(int i=0; i<NUM; i++){
    data[i]=rand()%MAX;
    if(i<100) printf("%5d%c",data[i],((i+1)%NRow==0)?'\n':' ');
    else if(i==100) printf("\n      ********\n");
    if(i>=NUM-100) printf("%5d%c",data[i],((i+1)%NRow==0)?'\n':' ');
  }

  stime=gettime();
  // Bubble sort
  for(int i=0; i<NUM-1; i++){
    for(int j=0; j<NUM-1; j++){
	if(data[j]>data[j+1]){
	  int tmp=data[j]; data[j]=data[j+1]; data[j+1]=tmp;
	}
    }
  }
  etime=gettime();

  printf("\n----- After Sort --%lfsec---\n",etime-stime);
  for(int i=0; i<100; i++)
    printf("%5d%c",data[i],((i+1)%NRow==0)?'\n':' ');
  printf("\n      ********\n");
  for(int i=NUM-100; i<NUM; i++)
    printf("%5d%c",data[i],((i+1)%NRow==0)?'\n':' ');
  printf("\n");

  return 0;
}
