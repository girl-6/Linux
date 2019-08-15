template<typename T>
class Singleton{ 
  static T data;   //提前准备好数据
public:
	static T* GetInstance(){
	 return &data;
	}
};
