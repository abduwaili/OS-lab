#define __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t*,sem);
_syscall1(int,sem_post,sem_t*,sem);
_syscall1(int,sem_unlink,const char *,name);

#define NUMBER 521
#define CONSUMER_NUM 5
#define BUF_LEN 10

int main()
{
  sem_t *empty, *full, *mutex;
  int fno, i, j, k, data, buf_in, buf_out;
  pid_t p;
  if((mutex=sem_open("mutex", 1))==NULL){
    perror("sem_open(mutex) error!\n");
    return -1;
  }
  if((empty=sem_open("empty", BUF_LEN))==NULL){
    perror("sem_open(empty) error!\n");
    return -1;
  }
  if((full=sem_open("full", 0))==NULL){
    perror("sem_open(full) error!\n");
    return -1;
  }
  fno=open("buffer.dat", O_CREAT|O_RDWR|O_TRUNC, 0666);
  lseek(fno, 10*sizeof(int), SEEK_SET);
  write(fno, (char *)&buf_out, sizeof(int));
  /* Producer */
  if((p=fork())==0){
    fflush(stdout);
    for(i=0; i<NUMBER; i++){
      sem_wait(empty);
      sem_wait(mutex);
      lseek(fno, buf_in*sizeof(int), SEEK_SET);
      write(fno, (char *)&i, sizeof(int));
      buf_in=(buf_in+1)%BUF_LEN;
      sem_post(mutex);
      sem_post(full);
    }
    return 0;
  }
  else if(p<0){
    perror("Faile to fork!\n");
    return -1;
  }
  for(j=0; j<CONSUMER_NUM; j++){
    if((p=fork())==0){
      for(k=0; k<NUMBER/CONSUMER_NUM; k++){
        sem_wait(full);
        sem_wait(mutex);
        lseek(fno, 10*sizeof(int), SEEK_SET);
        read(fno, (char*)&buf_out, sizeof(int));
        lseek(fno, buf_out*sizeof(int), SEEK_SET);
        read(fno, (char*)&data, sizeof(int));
        buf_out=(buf_out+1)%BUF_LEN;
        lseek(fno, 10*sizeof(int), SEEK_SET);
        write(fno, (char*)&buf_out, sizeof(int));
        sem_post(mutex);
        sem_post(empty);
        printf("%d: %d\n", getpid(), data);
        fflush(stdout);
      }
      return 0;
    }
    else if(p<0){
      perror("Fail to fork!\n");
    }
  }
  wait(NULL);
  sem_unlink("full");
  sem_unlink("empty");
  sem_unlink("mutex");
  close(fno);
  return 0;
}
