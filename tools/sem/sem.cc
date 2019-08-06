#include  <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM 16
using namespace std;

class RingQueue{
private:
  vector<int> q;  //模拟环形对列数组
  int cap;  //资源的数量
  sem_t data_sem;  //数据
  sem_t space_sem;  //空位
  int consume_step;  //消费者所在位置
  int product_step;  //生产者所在位置
public:
  RingQueue(int _cap=NUM):q(_cap),cap(_cap)
  {
    sem_init(&data_sem,0,0);
    sem_init(&space_sem,0,cap);
    consume_step=0;
    product_step=0;
  }
  void PutData(const int &data)  //生产者行为
  {
    sem_wait(&space_sem);  //等待信号对应减一
    q[product_step]=data;
    product_step++;
    product_step%=cap;  //保证不越界
    sem_post(&data_sem);  //发布信号对应加一
  }
  void GetData(int &data)   //消费者行为
  {
    sem_wait(&data_sem);  //等待数据
    data=q[consume_step];
    consume_step++;
    consume_step%=cap;
    sem_post(&space_sem);  //发布空位
  }
  ~RingQueue()
  {
    sem_destroy(&data_sem);
    sem_destroy(&space_sem);
  }
};

void *consume(void *arg)
{
  RingQueue *rq=(RingQueue*)arg;
  int data;
  while(1)
  {
    rq->GetData(data);
    cout<<"Consume data done:"<<data<<endl;
    sleep(1);
  }
}
void *product(void *arg)
{
  RingQueue *rq=(RingQueue*)arg;
  srand((unsigned long)time(NULL));
  while(1)
  {
    int data=rand()%100+1;
      rq->PutData(data);
    cout<<"Product data done:"<<data<<endl;
    //sleep(1);
  }
}

int main()
{
  RingQueue rq;
  pthread_t c,p;
  pthread_create(&c,NULL,consume,(void*)&rq);
  pthread_create(&p,NULL,product,(void*)&rq);
  pthread_join(c,NULL);
  pthread_join(p,NULL);
  return 0;
}
