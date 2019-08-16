#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *handle(void *arg)
{
	while(1)
	{
		cout<<"i am new"<<endl;
		sleep(1);
	}

}
int main()
{
	pthread_t tid;
	pthread_create(&tid,NULL,handle,(void*)"thread 1");
	while(1)
	{
		cout<<"i am main"<<endl;
		sleep(2);
	}
	return 0;
}


