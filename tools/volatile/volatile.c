#include <stdio.h>
#include <signal.h>
#include <unistd.h>

//volatile int flag=0;
int flag=0;
void sighandler(int sig)
{
	printf("change flag 0 to 1\n");
	flag=1;
}
int main()
{
	signal(2,sighandler);
	while(!flag);
//之后不能有代码，否则编译器会优化代码
	return 0;
}
