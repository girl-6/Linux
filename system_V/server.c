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
	key_t k=ftok(PATH,PRJ_ID);
	if(k<0)
	{
		printf("ftok error\n");
		return 1;
	}
	int shmid=shmget(k,SIZE,IPC_CREAT | IPC_EXCL | 0644);  //创建共享内存段
	if(shmid<0)
	{
		printf("shmget error\n");
		return 2;
	}
	char* p=(char*)shmat(shmid,NULL,0);   //建立映射
	int i=0;
	while(i<SIZE)
	{
		printf("client# %s\n",p);   //读取共享区数据
		sleep(1);
		++i;
	}
	shmdt(p);   //脱离映射
	//shmctl(shm_id,IPC_RMID,NULL);  //删除IPC资源
	return 0;
}
