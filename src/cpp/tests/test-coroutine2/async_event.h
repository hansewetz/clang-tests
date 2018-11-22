#pragma once
#include <iostream>
#include <stdexcept>
#include <atomic>
#include <experimental/coroutine>

// event class
class async_event{
public:
  using handle_t=std::experimental::coroutine_handle<>;

  // struct awater that goes togeter with 'async_event::co_await' operator
  class awaiter{
  friend class async_event;
  public:
    awaiter(async_event const&event):event_(event){}
    bool await_ready()const noexcept{return event_.is_set();}
    bool await_suspend(handle_t handle)noexcept{
      void const*setstate=&event_;              // get value for 'set state'
      handle_=handle;
      void*oldval=event_.state_.load(std::memory_order_acquire);  // get head of list
      do{
        if(oldval==setstate)return false;          // resume coroutine
        next_=static_cast<awaiter*>(oldval);       // update list to point to at current head
      }while(!event_.state_.compare_exchange_weak(oldval,this,std::memory_order_release,std::memory_order_acquire));
      return true;                                 // remain suspended
    }
    void await_resume()const noexcept{
    }

  private:
    async_event const&event_;           // global event
    handle_t handle_;                   // handle for this coroutine
    awaiter*next_;                      // next awaiter in list held by event
  };
  // ctors
  async_event(bool initset=false)noexcept:state_(initset?this:nullptr){}
  async_event(async_event const&)=delete;
  async_event(async_event&&)=delete;
  async_event&operator=(async_event const&)=delete;
  async_event&operator=(async_event&&)=delete;

  // methods
  bool is_set()const noexcept{return state_.load(std::memory_order_acquire)==this;}
  void set()noexcept{
    void*oldval=state_.exchange(this,std::memory_order_acq_rel);
    auto*waiters=static_cast<awaiter*>(oldval);
    while(waiters){
      waiters->handle_.resume();
      waiters=waiters->next_;
    }
  }
  void reset()noexcept{
    void*oldval=this;
    state_.compare_exchange_strong(oldval,nullptr,std::memory_order_acquire);
  }

  // co_await operator
  awaiter operator co_await()const noexcept{
    return awaiter{*this};
  }
private:
  // points to list of awaiters if 'not set'
  // points to 'this' (i.e. event) if 'set'
  mutable std::atomic<void*>state_;
};
