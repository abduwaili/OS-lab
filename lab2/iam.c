#include <stdio.h>
#define __LIBRARY__
#include <unistd.h>
_syscall1(int,iam,const char *,name);

int main(int argc, char *argv[])
{
  if(argc<=1)
    {
      printf("error: input your name, please! \n ");
      return -1;
    }
  if(iam(argv[1])==-1)
    {
      printf("error: length limit is 23. \n");
      return -1;
    }
  return 0;
}
