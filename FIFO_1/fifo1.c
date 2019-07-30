#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define ERR_EXIT(m)\
	do\
{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)

int main()
{
	mkfifo("tp",0644);   //创建管道
	int infd,outfd;
	infd=open("abc",O_RDONLY);
	if(infd==-1)	ERR_EXIT("open");
	outfd=open("tp",O_WRONLY);
	if(outfd==-1)	ERR_EXIT("open");

	char buf[1024];
	int n;
	while((n=read(infd,buf,1024))>0)
		write(outfd,buf,n);
	close(infd);
	close(outfd);
	return 0;
}


