
#include "TwoDITwTopK.h"
#include <iostream>

int main() {

// Create object a
std::cout<<std::endl<<"> Creating new interval store A."<<std::endl;
TwoDITwTopK a;

// Insert intervals (id, minKey, maxKey, maxTimestamp)
std::cout<<std::endl<<"> Inserting intervals (id, minKey, maxKey, maxTimestamp) into A:"<<std::endl
<<"(0+2, a, m, 1)"<<std::endl
<<"(0+4, d, e, 3)"<<std::endl
<<"(1, b, d, 4)"<<std::endl
<<"(2, l, s, 5)"<<std::endl
<<"(3+2, g, n, 8)"<<std::endl
<<"(4, n, w, 12)"<<std::endl
<<"(5, i, z, 16)"<<std::endl
<<"(6, q, x, 21)"<<std::endl
<<"(7, h, i, 25)"<<std::endl
<<"(0+2, b, n, 26) <- rewrite"<<std::endl
<<"(5, h, j, 27) <- rewrite"<<std::endl
<<"(0+4, c, o, 28) <- rewrite"<<std::endl
<<"(8, b, t, 30)"<<std::endl;
a.insertInterval("0+2", "a", "m", 1);
a.insertInterval("0+4", "d", "e", 3);
a.insertInterval("1", "b", "d", 4);
a.insertInterval("2", "l", "s", 5);
a.insertInterval("3+2", "g", "n", 8);
a.insertInterval("4", "n", "w", 12);
a.insertInterval("5", "i", "z", 16);
a.insertInterval("6", "q", "x", 21);
a.insertInterval("7", "h", "i", 25);
a.insertInterval("0+2", "b", "n", 26);
a.insertInterval("5", "h", "j", 27);
a.insertInterval("0+4", "c", "o", 28);
a.insertInterval("8", "b", "t", 30);

//
std::cout<<std::endl<<"> Tree in A:"<<std::endl;
a.treePrintLevelOrder();

// Call top-k
std::cout<<std::endl<<"> Top-5 intervals that overlap with (n,o) in A:"<<std::endl;
TwoDInterval r;
TopKIterator it(a, r, "n", "o"); // Locks a for inserts, deletes and iteration by other iterators

// Top 2 intervals:
int index = 0;
while(it.next()) {
  std::cout<<"("<<r.GetId()<<", "<<r.GetLowPoint()<<", "<<r.GetHighPoint()<<", "<<r.GetTimeStamp()<<")"<<std::endl;
  if (++index == 2)
    break;
}

// Here a is still locked 

// Rest of the top 5 intervals:
while(it.next()) {
  std::cout<<"("<<r.GetId()<<", "<<r.GetLowPoint()<<", "<<r.GetHighPoint()<<", "<<r.GetTimeStamp()<<")"<<std::endl;
  if (++index == 5)
    break;
}

// Insert releases a for inserts, deletes and iteration by other iterators
std::cout<<std::endl<<"> Inserting interval (0+3, e, z, 32) into A:"<<std::endl;
a.insertInterval("0+3", "e", "z", 32);

// Restart it with new search interval
std::cout<<std::endl<<"> All top intervals that overlap with (h,p) in A:"<<std::endl;
it.restart("h","p");
while(it.next())
  std::cout<<"("<<r.GetId()<<", "<<r.GetLowPoint()<<", "<<r.GetHighPoint()<<", "<<r.GetTimeStamp()<<")"<<std::endl;
it.stop(); //release a

// Delete interval (id)
std::cout<<std::endl<<"> Deleting interval with id 4 (i.e. (n,w)) in A."<<std::endl;
a.deleteInterval("4");

//
std::cout<<std::endl<<"> Tree in A:"<<std::endl;
a.treePrintLevelOrder();

//
std::cout<<std::endl<<"> All top intervals that overlap with (m,o) in A:"<<std::endl;
it.restart("m","o");
while(it.next())
  std::cout<<"("<<r.GetId()<<", "<<r.GetLowPoint()<<", "<<r.GetHighPoint()<<", "<<r.GetTimeStamp()<<")"<<std::endl;
it.stop(); //release a

// Delete all intervals with common prefix (id_prefix)
std::cout<<std::endl<<"> Deleting all intervals with ids starting with 0 (i.e. (b,n) and (c,o)) in A."<<std::endl;
a.deleteAllIntervals("0");

//
std::cout<<std::endl<<"> Tree in A:"<<std::endl;
a.treePrintLevelOrder();

//
std::cout<<std::endl<<"> All top intervals that overlap with (m,o) in A:"<<std::endl;
it.restart("m","o");
while(it.next())
  std::cout<<"("<<r.GetId()<<", "<<r.GetLowPoint()<<", "<<r.GetHighPoint()<<", "<<r.GetTimeStamp()<<")"<<std::endl;
it.stop(); //release a

std::cout<<std::endl;

};

