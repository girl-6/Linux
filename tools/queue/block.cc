#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

using namespace std;

class BlockQueue
{
  private:
    queue<int> q;
    pthread_cond_t cond;
    pthread_mutex_t lock;
  private:
    void LockQueue()
    {
      pthread_mutex_lock(&lock);
    }
    void UnlockQueue()
    {
      pthread_mutex_unlock(&lock);
    }
    bool isEmpty()
    {
      return q.size()==0?true:false;
    }
    void WakeConsume()
    {
      pthread_cond_signal(&cond);
    }
    void ProductWait()
    {
      pthread_cond_wait(&cond,&lock);
    }
  public:
    BlockQueue()
    {
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&cond,NULL);
    }
    void PushData(const int &data)  //生产者生产，并通知消费者
    {
      LockQueue();
      q.push(data);
      UnlockQueue();
      cout<<"product run done,data push sucess:"<<data<<endl;
      WakeConsume();
    }
    void PopData(int &data)  //消费者消费，无法消费时等待
    {
      LockQueue();
      while(isEmpty())  //用while可防止误唤醒同时等待条件满足
        ProductWait();
      data=q.front();
      q.pop();
      UnlockQueue();
      cout<<"consume run done,data pop sucess:"<<data<<endl;
    }
    ~BlockQueue()
    {
      pthread_cond_destroy(&cond);
      pthread_mutex_destroy(&lock);
    }

};
    void* product (void *arg) //生产行为
    {
      BlockQueue *bq=(BlockQueue*)arg;
      srand((unsigned long)time(NULL));
      while(1)
      {
        int data=rand()%100+1;
        bq->PushData(data);
        sleep(1);
      }
    }
    void* consume(void *arg)   //消费行为
    {
      BlockQueue *bq=(BlockQueue*)arg;
      while(1)
      {
        int d;
        bq->PopData(d);
      }
    }


int main()
{
  BlockQueue bq;
  pthread_t p,c;
  pthread_create(&p,NULL,product,(void*)&bq);
  pthread_create(&c,NULL,consume,(void*)&bq);
  pthread_join(p,NULL);
  pthread_join(c,NULL);
  return 0;
}
