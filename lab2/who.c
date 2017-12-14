#define __LIBRARY__
#include <asm/segment.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kernel.h>
#define MAX_SIZE 23

char username[MAX_SIZE+1];

int sys_iam(const char *name)
{
  int i;
  for(i=0; get_fs_byte(name+i)!='\0'; i++);
  printk("sys_iam:\nname size is:%d \n", i);
  if(i>MAX_SIZE)
    return -EINVAL;
  for(i=0; (username[i]=get_fs_byte(name+i))!='\0'; i++);
  return i;
}

int sys_whoami(char *name, unsigned int size)
{
  int i;
  for(i=0; put_fs_byte(username[i], name+i), username[i]!='\0'; i++);
  printk("sys_whoami:\nusername size is:%d \ngiven size is:%d\n", i, size);
  if(i>size)
    return -EINVAL;
  return i;
}
