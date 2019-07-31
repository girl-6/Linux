#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define ERR_EXIT(m)\
	do{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

int main()
{
	umask(0);
	int wfd=open("myfifo",O_WRONLY); //只写方式打开管道
	if(wfd<0)
		ERR_EXIT("open\n");

	char buf[1024];
	while(1)
	{
		buf[0]=0;
		printf("Please Enter#:");
		fflush(stdout);
		ssize_t s=read(0,buf,sizeof(buf)-1);
		if(s>0)   //读取成功
		{
			buf[s]=0;  //给字符串最后补充 \0
			write(wfd,buf,strlen(buf));
		}
		else  //读取文件失败
			ERR_EXIT("read error\n");
	}
	close(wfd);
	return 0;
}
