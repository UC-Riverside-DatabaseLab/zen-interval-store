
#include "TwoD_IT_w_TopK.h"
#include <iostream>

int main() {

TwoD_IT_w_TopK a;

// Insert intervals (id, minKey, maxKey, maxTimestamp)
std::cout<<std::endl<<"Inserting intervals:"<<std::endl
         <<"(a,m,1)"<<std::endl
         <<"(b,d,2)"<<std::endl
         <<"(l,s,3)"<<std::endl
         <<"(g,o,4)"<<std::endl
         <<"(n,w,5)"<<std::endl
         <<"(i,z,6)"<<std::endl
         <<"(q,x,7)"<<std::endl
         <<"(h,i,8)"<<std::endl
         <<"(b,t,9)"<<std::endl;
a.insertInterval("0", "a", "m", 1);
a.insertInterval("1", "b", "d", 2);
a.insertInterval("2", "l", "s", 3);
a.insertInterval("3", "g", "o", 4);
a.insertInterval("4", "n", "w", 5);
a.insertInterval("5", "i", "z", 6);
a.insertInterval("6", "q", "x", 7);
a.insertInterval("7", "h", "i", 8);
a.insertInterval("8", "b", "t", 9);

// Call top-k (minKey, maxKey, k)
std::cout<<std::endl<<"Top-4 intervals that overlap with (n,o):"<<std::endl;
std::vector<TwoD_Interval> r;
a.topK(&r, "n", "o", 4);
for(std::vector<TwoD_Interval>::const_iterator it = r.begin(); it != r.end(); it++) {
  std::cout<<"("<<it->GetLowPoint()<<","<<it->GetHighPoint()<<","<<it->GetMaxTimeStamp()<<")"<<std::endl;
  }

// Delete interval (id)
std::cout<<std::endl<<"Deleting (n,w)"<<std::endl;
a.deleteInterval("4");

std::cout<<std::endl<<"Top-4 intervals that overlap with (n,o):"<<std::endl;
r.clear();
a.topK(&r, "n", "o", 4);
for(std::vector<TwoD_Interval>::const_iterator it = r.begin(); it != r.end(); it++) {
  std::cout<<"("<<it->GetLowPoint()<<","<<it->GetHighPoint()<<","<<it->GetMaxTimeStamp()<<")"<<std::endl;
  }

std::cout<<std::endl;

return 0;
};
