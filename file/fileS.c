#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	close(2);
	int fd=open("newfile",O_RDONLY|O_CREAT);
	if(fd<0)
	{
		perror("open");
		return 1;
	}
	printf("fd:%d\n",fd);
	close(fd);
	return 0;
}
