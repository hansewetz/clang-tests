#include "test1.h"
#include <iostream>
#include <experimental/coroutine>
using namespace std;

// test main program
int main(){
  auto numgen=[](int n)->gen<int>{
      for(int i=0;i<n;++i)co_yield i;
    };
  auto g=numgen(5);
//  copy(begin(g),end(g),ostream_iterator<int>(cout, " "));
//  cout<<endl;
  auto g1=numgen(10);
  while(g1.next())cout<<g1.value()<<" "<<endl;
}
