
#include "TwoDITwTopK.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>

int main() {

// Create object a
std::cout<<std::endl<<"> Creating new interval store A."<<std::endl;
TwoDITwTopK a;

srand(time(NULL));
std::string id, min, max;
int file_num=0, block_num=0, n1, n2, num_blocks=700+(rand()%600);
uint64_t e=0;
//std::chrono::time_point start_, end_;
std::ofstream o1("insert.perf"), o2("deleteAll.perf"), o3("topK.perf");

// Insert intervals (id, minKey, maxKey, maxTimestamp)
std::cout<<std::endl<<"> Inserting 1,000,000 intervals (id, minKey, maxKey, maxTimestamp) into A:"<<std::endl;
for (int i = 0; i < 1000000; i++) {
  
  id = std::to_string(file_num) + '+' + std::to_string(block_num);
  n1 = rand() % 100000;
  n2 = rand() % 1000;
  min = std::to_string(n1);
  max = std::to_string((n2 < 100000) ? n2 : 99999);
  
  block_num++;
  if (block_num == num_blocks) {
    file_num++;
    block_num = 0;
    num_blocks = 700+(rand()%600);
  }
  
    auto start_ = std::chrono::system_clock::now();
    a.insertInterval(id, min, max, i);
    auto end_ = std::chrono::system_clock::now();
    auto elapsed = end_ - start_;
    e += elapsed.count();
  if (i % 1000 == 0) {
    std::cout<<i<<std::endl;
    o1<<i<<'\t'<<e<<std::endl;
    e=0;
  }
  else
    a.insertInterval(id, min, max, i);
  

  if (rand() % 10000 < 1) {
    id = std::to_string(rand()%(file_num+1));
    //std::cout<<"\nDeleting all intervals starting with: "<<id<<"";
    auto start_2 = std::chrono::system_clock::now();
    a.deleteAllIntervals(id);
    auto end_2 = std::chrono::system_clock::now();
    auto elapsed = end_2 - start_2;
    o2<<i<<'\t'<<elapsed.count()<<std::endl;
  }
  

  if (rand() % 1000 < 1) {
  n1 = rand() % 100000;
  n2 = rand() % 100;
  min = std::to_string(n1);
  max = std::to_string((n2 < 100000) ? n2 : 99999);
    //std::cout<<"\nDeleting all intervals starting with: "<<id<<"";
    auto start_3 = std::chrono::system_clock::now();
    int index = 0;
    TwoDInterval r;
    TopKIterator it(a, r, min, max);

while(it.next()) {
  r.GetId();
  r.GetLowPoint();
  r.GetHighPoint();
  r.GetTimeStamp();
  if (++index == 5)
    break;
}

    auto end_3 = std::chrono::system_clock::now();
    auto elapsed = end_3 - start_3;
    o3<<i<<'\t'<<elapsed.count()<<std::endl;
  }
  
}

o1.close();
o2.close();
o3.close();
}


