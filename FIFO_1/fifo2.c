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
	int infd, outfd;
	outfd=open("abc.bak",O_WRONLY|O_CREAT);
	if(outfd==-1)  ERR_EXIT("open");
	infd=open("tp",O_RDONLY);
	if(infd==-1)	ERR_EXIT("open");

	char buf[1024];
	int n;
	while((n=read(infd,buf,1024))>0)
		write(outfd,buf,n);

	close(infd);
	close(outfd);
	unlink("tp");
	return 0;
}
