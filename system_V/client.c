#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PATH "/tmp"
#define PRJ_ID 0x6666
#define SIZE 4096
int main()
{
	key_t k=ftok(PATH,PRJ_ID);  //得到共享内存段
	if(k<0)
	{
		printf("ftok error\n");
		return 1;
	}
	int shmid=shmget(k,SIZE,0);
	if(shmid<0)
	{
		printf("shmget error\n");
		return 2;
	}
	char *p=(char*)shmat(shmid,NULL,0); //创建映射关系
	int i=0;
	while(i<SIZE)
	{
		p[i]='a'+i;   //写入共享内存段
		i++;
		p[i]=0;
		sleep(1);
	}
	shmdt(p);   //脱离映射
	return 0;
}
