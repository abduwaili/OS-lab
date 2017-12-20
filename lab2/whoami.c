#include <stdio.h>
#define __LIBRARY__
#include <unistd.h>
_syscall2(int,whoami,char *,name,unsigned int,size);

int main(int argc, char *argv[])
{
  char name[24];
  if(whoami(name, 23)==-1){
    printf("error while reading name...");
    return -1;
  }
  printf("%s\n", name);
  return 0;
}
