#include <stdio.h>  
#include <string.h>
#include <error.h>
int main()
{
	FILE *fp=fopen("myfile","r");
	if(!fp)
	{
		perror("fopen");
	}
	char buf[1024];
	const char *msg="hello world!\n";
	while(1)
	{
		ssize_t s=fread(buf,1,strlen(msg),fp);
		if(s>0)
		{
			buf[s]=0;
			printf("%s",buf);
		}
		if(feof(fp))
		{
			break;
		}
	}
	fclose(fp);
	return 0;
}

