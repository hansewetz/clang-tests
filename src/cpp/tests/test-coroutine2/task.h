#pragma once
#include <iostream>
#include <stdexcept>
#include <experimental/coroutine>

template<typename T>class task;
template<>class task<void>{
public:
  // typedefs
  struct promise_type;
  using handle_t=std::experimental::coroutine_handle<promise_type>;

  // promise_type for coroutine
  struct promise_type{
    void return_void(){};
    auto initial_suspend(){
      return std::experimental::suspend_never{};
    }
    auto final_suspend(){
      return std::experimental::suspend_always{};
    }
    task get_return_object(){return task(handle_t::from_promise(*this));}  // get coroutine object (from promise)
    void unhandled_exception(){std::terminate();}                        // what to do if we have an exception slipping out of the coroutine
  };
  // move constructor
  // (note: handle_ is not a pointer but can be assigned a 'nullptr' --> assgn operator)
  task(task const&)=delete;
  task(task&&other):handle_(std::move(other.handle_)){other.handle_=nullptr;}
  task&operator=(task const&)=delete;
  task&operator=(task&&other){
    handle_=other.handle_;
    other.handle_=nullptr;
    return *this;
  }
  // destructor
  ~task(){
    if(handle_)handle_.destroy();
  }
  // get value
  void resume(){handle_.resume();}
private:
  // ctor - called from promise.get_return_object()
  explicit task(handle_t handle):handle_(handle){}

  // state of this coroutine object
  handle_t handle_;
};

