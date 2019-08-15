template <typename T>

class Singleton{ 
	volatile static T* inst;  //防止被优化
	static std::mutex lock;
public:
	static T* GetInstance()
	{
		if(inst==NULL)  //双重判空，降低锁的冲突，提高性能
		{
			lock.lock();  //保证多线程下也只new一次
			if(inst==NULL)
				inst=new T();
			lock.unlock();
		}
		return inst;
	}
};
