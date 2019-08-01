#include <stdio.h>

int main(int argc,char*argv[],char* env[])
{
	int i=0;
	for(;env[i];i++)
	{
		printf("%s\n",env[i]);
	}
	return 0;
}
