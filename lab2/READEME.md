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