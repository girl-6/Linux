#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PATH "/tmp"
#define PROJ_ID 0x6666
#define SIZE 4096


static int commShm(int size,int flags)
{
	key_t key=ftok(PATH,PROJ_ID);
	if(key<0)
	{
		perror("ftok");
		return 1;
	}
	int shmid=0;
	if(shmid=shmget(key,size,flags)<0)
	{
		perror("shmget");
		return 2;
	}
	return shmid;
}

int destoryShm(int shmid)
{
	if(shmctl(shmid,IPC_RMID,NULL)<0)
	{
		perror("shmctl");
		return 1;
	}
	return 0;
}

int creatShm(int size)
{
	return commShm(size,IPC_CREAT | IPC_EXCL | 0644);
}

int getShm(int size)
{
	return commShm(size,0);
}

