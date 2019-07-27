#include "Baymax.hpp"
#define LOG_PATH "./log.txt"

int main()
{
#ifdef _LOG_   //将所有cerr 输出的错误信息归到文件中
	int fd=open(LOG_PATH,O_WRONLY | O_CREAT,0644);
	if(fd<0)
		std::cerr<<"open file error!"<<std::endl;
	dup2(fd,2);
#endif

	Baymax *bm = new Baymax;
	bm->Run();

#ifdef _LOG_
	close(fd);
#endif

	return 0;
}
