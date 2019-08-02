#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler(int sig)
{
	pid_t id;
	while(id=waitpid(-1,NULL,WNOHANG)>0)
	{
		printf("wait child success:%d\n",id);
	}
	printf("child quit\n");
}
int main()
{
	signal(SIGCHLD,handler);
	pid_t cid;
	if(cid=fork()==0) //chid
	{
		printf("child :%d\n",getpid());
		sleep(3);
		exit(1);
	}
	while(1)
	{
		printf("father doing other\n");
		sleep(1);
	}
	return 0;
}
