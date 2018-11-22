#include "async_event.h"
#include "task.h"
#include <iostream>
#include <thread>
#include <experimental/coroutine>
#include <unistd.h>
using namespace std;

// globals
async_event event;

// producer
void producer(){
  cout<<"#producer: sleeping, id: "<<std::this_thread::get_id()<<endl;
  sleep(1);
  cout<<"#producer: setting event, id: "<<std::this_thread::get_id()<<endl;
  event.set();
  cout<<"#producer: done, id: "<< std::this_thread::get_id()<<endl;
}
task<void>consumer(){
  cout<<"#consumer - tid: "<<std::this_thread::get_id()<<", before 'co_await'"<<endl;
  co_await event;  
  // ...
  // after co_await we continue on the producer thread (see: async_event::set() method
  // ...
  cout<<"#consumer - tid: "<<std::this_thread::get_id()<<", after 'co_await'"<<endl;
}

// test main program
int main(){
  thread tproducer(producer);
  auto task1=consumer();
  auto task2=consumer();
  auto task3=consumer();

  cout<<"#joining, id: "<<std::this_thread::get_id()<<endl;
  tproducer.join();
  cout<<"#joined, id: "<<std::this_thread::get_id()<<endl;
}
