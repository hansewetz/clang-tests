#include "test1.h"
#include <iostream>
#include <experimental/coroutine>
using namespace std;

// test main program
int main(){
  auto numgen=[](int n)->gen<int>{
      for(int i=0;i<n;++i)co_yield i;
    };
  auto g1=numgen(5);
  auto g=std::move(g1);
  copy(begin(g),end(g),ostream_iterator<int>(cout, " "));
  cout<<endl;

  for(auto&&i:numgen(6))cout<<i<<" ";
  cout<<endl;

  auto g3=numgen(10);
  while(g3.next())cout<<g3()<<" ";
  cout<<endl;

  auto g2=numgen(10);
  while(g2.next())cout<<g2.value()<<" "<<endl;
}
