#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main()
{
	char buf[1024];
	ssize_t s=read(0,buf,sizeof(buf));
	if(s>0)
		{
			buf[s]=0;
			write(1,buf,strlen(buf));
			write(2,buf,strlen(buf));
		}
	return 0;
}

/*int main()
{
	int fd=open("myfiles",O_WRONLY|O_CREAT,0644);
	if(fd<0)
	{
		perror("open");
		return 1;
	}
	int count=5;
	const char*msg="hello world!\n";
	int len=strlen(msg);
	while(count--)
	{
		write(fd,msg,len);
	}

	close(fd);
	return 0;
}*/
