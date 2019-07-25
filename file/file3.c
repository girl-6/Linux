#include <stdio.h>
#include <string.h>

int main()
{
	const char* s1="hello printf\n";
	const char* s2="hello fwrite\n";
	const char* s3="hello write \n";

	printf("%s",s1);
	fflush(stdout);
	fwrite(s2,strlen(s2),1,stdout);
	write(1,s3,strlen(s3));

	fork();
	return 0;
}

