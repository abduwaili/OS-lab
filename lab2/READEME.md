# 系统调用

## 需要更改的系统文件

- include/unistd.h
- include/linux/sys.h
- kernel/system_call.s

## 需要自己添加的文件

- kernel/who.c

## 需要自己更改的工具文件

- kernel/Makefile

## 测试自己添加的系统调用函数功能的文件

- iam.c
- whoami.c

## 评分文件

- testlab2.c
- testlab2.sh

## 踩坑

- include/unistd.h 是所有应用程序需要的头文件，而 make Linux 内核之后这个文件并没有同时更改，所以直接编译 iam.c whoami.c testlab2.c 会出错，需要在 /usr/include 中的 unistd.h 加上和原来在 include/linux/sys.h 中添加的**相同**顺序的宏定义