#pragma once
#include <iostream>
#include <stdexcept>
#include <experimental/coroutine>

template<typename T>
class gen{
public:
  // typedefs
  struct promise_type;
  using handle_t=std::experimental::coroutine_handle<promise_type>;

  // promise_type for coroutine
  struct promise_type{
    T currval_;
    std::experimental::suspend_always yield_value(T val){            // called when 'yield' + returns an 'awaiter'
      currval_=val;
      return {};
    }
    std::experimental::suspend_always initial_suspend(){return {};}  // returns an 'awaiter'
    std::experimental::suspend_always final_suspend(){return {};}    // returns an 'awaiter'
    gen get_return_object(){return gen(this);}                       // get coroutine object (from promise)
    void unhandled_exception(){std::terminate();}                    // what to do if we have an exception slipping out of the coroutine
    void return_void(){};
  };
  // iterator interface
  class iterator{
  public:
    iterator(handle_t handle,bool done):
        handle_(handle),done_(done){}
    iterator&operator++(){
      handle_.resume();
      done_=handle_.done();
      return*this;
    }
    bool operator==(iterator const&other)const{return done_==other.done_;}
    bool operator!=(iterator const&other)const{return !(*this==other);}
    T const&operator*()const{return handle_.promise().currval_;}
    T const*operator->(){return &(operator*());}
  private:
    handle_t handle_;
    bool done_;
  };
  iterator begin(){
    cohandle_.resume();
    return {cohandle_,cohandle_.done()};
  }
  iterator end(){
    return {cohandle_,true};
  }
  // next/value interface
  bool next(){
    if(cohandle_.done())return false;
    cohandle_.resume();
    return !cohandle_.done();
  }
  T const&value()const{return cohandle_.promise().currval_;}

  // ctor/ctor
  gen(gen&&other):cohandle_(other.cohandle_){other.cohandle_=nullptr;}
  ~gen(){
    if(cohandle_)cohandle_.destroy();
  }
private:
  // ctor - called from promise.get_return_object()
  gen(promise_type*prom):cohandle_(handle_t::from_promise(*prom)){}

  // state of this coroutine object
  handle_t cohandle_;
};
