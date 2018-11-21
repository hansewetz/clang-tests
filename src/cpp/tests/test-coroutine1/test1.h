#pragma once
#include <iostream>
#include <stdexcept>
#include <experimental/coroutine>

/*
  - iterator: allows user to manage 'gen' objects from 'outside'
  - promise+handle: allows 'gen' to manage coroutine from 'inside'
*/
template<typename T>
class gen{
public:
  // typedefs
  struct promise_type;
  using handle_t=std::experimental::coroutine_handle<promise_type>;

  // promise_type for coroutine
  // (constructed behind the scenes by the compiler)
  // (is a kind of factory for 'gen' objects: 'get_return_object(...) creates a 'gen' object)
  struct promise_type{
    T currval_;
    auto yield_value(T const&val){                 // called when 'yield' + returns an 'awaiter'
      currval_=val;                                // this is 'return' value (see: value() and iterator::operator*())
      return std::experimental::suspend_always{};  // note: 'co_yield t' <===> 'co_await promise.yield_value(t)'
    }
    auto initial_suspend(){return std::experimental::suspend_always{};}  // returns an 'awaiter'
    auto final_suspend(){return std::experimental::suspend_always{};}    // returns an 'awaiter'
    gen get_return_object(){return gen(handle_t::from_promise(*this));}  // get coroutine object (from promise)
    void unhandled_exception(){std::terminate();}                        // what to do if we have an exception slipping out of the coroutine
    void return_void(){};
  };
  // iterator interface
  class iterator{
  public:
    iterator(handle_t handle,bool done):handle_(handle),done_(done){
    }
    iterator&operator++(){
      handle_.resume();      // can call 'handle_()' also
      done_=handle_.done();
      return*this;
    }
    iterator&operator++(int)=delete;
    bool operator==(iterator const&other)const{return done_==other.done_;}
    bool operator!=(iterator const&other)const{return !(*this==other);}
    T const&operator*()const{return handle_.promise().currval_;}
    T const*operator->(){return &(operator*());}
  private:
    handle_t handle_;
    bool done_;
  };
  // the 'gen' class acts as a container prducing 'begin()' and 'end()' iterators
  iterator begin(){
    if(!handle_)return end();       // note: coroutine_handle::operator=(nullptr_t)
    handle_.resume();               // can call 'handle_()' also
    return {handle_,handle_.done()};
  }
  iterator end(){
    return {handle_,true};
  }
  // next/value interface
  bool next(){
    if(handle_.done())return false;
    handle_.resume();      // can call 'handle_()' also
    return !handle_.done();
  }
  T const&value()const{return handle_.promise().currval_;}
  T const&operator()()const{return value();}

  // move constructor
  // (note: handle_ is not a pointer but can be assigned a 'nullptr' --> assgn operator)
  gen(gen const&)=delete;
  gen(gen&&other):handle_(std::move(other.handle_)){other.handle_=nullptr;}
  gen&operator=(gen const&)=delete;
  gen&operator=(gen&&other){
    handle_=other.handle_;
    other.handle_=nullptr;
    return *this;
  }
  // destructor
  ~gen(){
    if(handle_)handle_.destroy();
  }
private:
  // ctor - called from promise.get_return_object()
  explicit gen(handle_t handle):handle_(handle){}

  // state of this coroutine object
  handle_t handle_;
};
