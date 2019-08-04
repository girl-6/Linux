#include <stdio.h>
#include <signal.h>

void handler(int sig)
{
	sleep(5);
	printf("catch a sig:%d\n",sig);
}

int main()
{
	signal(SIGSEGV,handler);
	int *p=NULL;
	*p=100;
	sleep(1);
	while(1);
	return 0;
}

