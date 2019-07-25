#include<iostream>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include<queue>
using namespace std;

int cal(int x, int y, int op);
typedef int(*HandlerTask_t) (int x, int y, int op);
class Task{
	private:
		int x;
		int y;
		int op;                    //0+ 1- 2* 3/
		HandlerTask_t handler;     //处理方法
	public:
		Task(int _x = -1, int _y = -1, int _op = -1) 
			:x(_x), y(_y), op(_op)
		{}
		void Register(HandlerTask_t _handler)     //可从外界注册处理方法
		{
			handler = _handler;
		}
		void Run()            //怎么进行处理任务
		{
			int ret = handler(x, y, op);
			const char* arr = "+-*/";
			cout << pthread_self() << " : " << x << arr[op] << y << "=" << ret << endl;    //打印结果
		}
		~Task(){}
};

class ThreadPool{
	private:
		int thread_nums;     //线程池中的线程总数
		int idle_nums;       //当前闲置线程的数量
		queue<Task> task_queue;     //线程所要处理的任务队列
		pthread_mutex_t lock;      //加锁
		pthread_cond_t cond;       //条件变量
	public:
		void LockQueue()         //锁任务队列
		{
			pthread_mutex_lock(&lock);
			}
			void UnlockQueue()       //解锁
		{
			pthread_mutex_unlock(&lock);
		}
		bool QueueIsEmpty()      //判断
		{
			return task_queue.size() == 0 ? true : false;
		}
		void ThreadIdle()           
		{
			idle_nums++;           //一旦进入等待状态闲置线程加一
			pthread_cond_wait(&cond, &lock);       //进程等待
			idle_nums--;          //被唤醒
		}
		void WakeupThread()     //唤醒线程
		{
			pthread_cond_signal(&cond);
		}
		void PopTask(Task& t)     //出任务
		{
			t = task_queue.front();
			task_queue.pop();
		}
	public:
		ThreadPool(int _num = 5) :thread_nums(_num), idle_nums(0)  //初始化线程池大小为5，闲置为0
	{
		pthread_mutex_init(&lock, NULL);
		pthread_cond_init(&cond, NULL);
	}
		static void* ThreadRotinue(void* arg)    //处理过程，非静态的成员函数会导致多一个参数,所以写成静态函数
		{
			pthread_detach(pthread_self());     //分离线程
			ThreadPool *tp = (ThreadPool*)arg;    //创建线程指针即this，以后都用tp代替this指针
			for (;;){
				tp->LockQueue();
				while (tp->QueueIsEmpty()){      
					tp->ThreadIdle();         //任务队列为空时等待任务
				}
				Task t;
				tp->PopTask(t);          //有任务就直接取走
				tp->UnlockQueue();
				t.Run();             //处理任务的方法,放在锁外可提高处理任务的效率，放在锁内会导致其他线程饥饿
			}
		}
		void InitThreadPool()      //创建所需要的线程
		{
			pthread_t t;
			for (auto i = 0; i < thread_nums; i++)
			{
				pthread_create(&t, NULL, ThreadRotinue, this);     //this指针是函数ThreadRotinue的参数
			}
		}
		void PushTask(const Task& t)        //在线程池中放入任务
		{
			LockQueue();       
			task_queue.push(t);         
			WakeupThread();              //唤醒线程
			UnlockQueue();
		}
		~ThreadPool()
		{
			pthread_mutex_destroy(&lock);
			pthread_cond_destroy(&cond);
		}
};

int cal(int x, int y, int op)   //处理函数
{
	int ret = -1;
	switch (op){
		case 0:
			ret = x + y;
			break;
		case 1:
			ret = x - y;
			break;
		case 2:
			ret = x * y;
			break;
		case 3:
			ret = x / y;
			break;
		default:
			std::cout << "cal error!" << std::endl;
	}
	return ret;
}
int main()
{
	ThreadPool tp;
	tp.InitThreadPool();    //初始化线程池
	srand((unsigned long)time(NULL));
	for (;;){
		sleep(1);    //每隔一秒放入一个任务
		int x = rand() % 100 + 1;
		int y = rand() % 100 + 1;
		int op = rand() % 4;
		Task t(x, y, op);
		t.Register(cal);      //给任务注册方法
		tp.PushTask(t);      //将任务放入线程池
	}
	return 0;
}

