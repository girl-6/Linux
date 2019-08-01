# include <stdio.h>
# include <sys/types.h>
#include <unistd.h>

int main()
{
	printf("ppid:%d\n",getppid());
	printf("pid:%d\n",getpid());
	return 0;
}
