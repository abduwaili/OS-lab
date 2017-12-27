#define __LIBRARY__
#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>


#define SEM_NUM 32
sem_t semaphores[SEM_NUM];
/* 初始化进程等待队列 */
void init_queue(sem_queue *q)
{
  q->front=q->rear=0;
}
/* 判断队列是否为空 */
int is_empty(sem_queue *q)
{
  return q->front==q->rear;
}
/* 判断队列是否已满 */
int is_full(sem_queue *q)
{
  return (q->rear+1)%QUE_LEN==q->front;
}
/* 获得队首进程 */
struct task_struct* get_task(sem_queue* q)
{
  if(is_empty(q)){
    printk("Queue is empty!\n");
    return NULL;
  }
  struct task_struct *tmp=q->wait_tasks[q->front];
  q->front=(q->front+1)%QUE_LEN;
  return tmp;
}
/* 队尾插入进程 */
int insert_task(struct task_struct *p, sem_queue *q)
{
  if(is_full(q)){
    printk("Queue is full!\n");
    return -1;
  }
  q->wait_tasks[q->rear]=p;
  q->rear=(q->rear+1)%QUE_LEN;
  return 1;
}
/* 查看信号量是否存在返回位置 */
int sem_location(const char name[])
{
  int i;
  for(i=0; i<SEM_NUM; i++){
    if(strcmp(name, semaphores[i].name)==0&&semaphores[i].occupied){
      return i;
    }
  }
  return -1;
}
/* 打开信号量 */
sem_t *sys_sem_open(const char name[], unsigned int value)
{
  char tmp[NAME_LEN+1], c;
  int i;
  for(i=0; i<=NAME_LEN; i++){
    c=get_fs_byte(name+i);
    tmp[i]=c;
    if(c=='\0')break;
  }
  if(i>NAME_LEN){
    printk("Semaphore name is too long!\n");
    return NULL;
  }
  if((i=sem_location(tmp))!=-1){
    printk("sem existed!\n");
    return &semaphores[i];
  }
  for(i=0; i<SEM_NUM; i++){
    if(!semaphores[i].occupied){
      strcpy(semaphores[i].name, tmp);
      semaphores[i].occupied=1;
      semaphores[i].value=value;
      init_queue(&(semaphores[i].wait_queue));
      // printk("sem create succesfully!\n");
      // printk("%p\n", &semaphores[i]);
      return &semaphores[i];
    }
  }
  printk("Number of semaphores is limited!\n");
  return NULL;
}
/* P原子操作 */
int sys_sem_wait(sem_t *sem)
{
  cli();
  sem->value--;
  if(sem->value<0){
    current->state=TASK_UNINTERRUPTIBLE;
    insert_task(current, &(sem->wait_queue));
    schedule();
  }
  sti();
  return 0;
}
/* V原子操作 */
int sys_sem_post(sem_t* sem)
{
  cli();
  struct task_struct *p;
  sem->value++;
  if(sem->value<=0){
    p=get_task(&(sem->wait_queue));
    if(p!=NULL){
      (*p).state=TASK_RUNNING;
    }
  }
  sti();
  return 0;
}
/* 释放信号量 */
int sys_sem_unlink(const char name[])
{
  char tmp[NAME_LEN+1], c;
  int i;
  for(i=0; i<=NAME_LEN; i++){
    c=get_fs_byte(name[i]);
    tmp[i]=c;
    if(c=='\0')break;
  }
  if(c=='\0'){
    printk("Semphore name is too long!\n");
    return -1;
  }
  int ret=sem_location(tmp);
  if(ret!=-1){
    semaphores[ret].value=0;
    strcpy(semaphores[ret].name, "\0");
    semaphores[ret].occupied=0;
    return 0;
  }
  return -1;
}
