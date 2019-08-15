template <typename T>
class Singleton{
	static T* inst;   //声明
	public:
	static T* GetInstance(){ 
	if(inst==NULL)
		inst =new T();   //用的时候再创建
	return inst;
	}
};

