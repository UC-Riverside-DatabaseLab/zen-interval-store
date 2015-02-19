
#include "TwoDITwTopK.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {

// Create object a
std::cout<<std::endl<<"> Creating new interval store A."<<std::endl;
TwoDITwTopK a;

srand(time(NULL));
std::string id, min, max;
int file_num=0, block_num=0, n1, n2, num_blocks=700+(rand()%600);

// Insert intervals (id, minKey, maxKey, maxTimestamp)
std::cout<<std::endl<<"> Inserting 1,000,000 intervals (id, minKey, maxKey, maxTimestamp) into A:"<<std::endl;
for (int i = 0; i < 1000000; i++) {
  
  id = std::to_string(file_num) + '+' + std::to_string(block_num);
  n1 = rand() % 100000;
  n2 = rand() % 100000;
  min = std::to_string((n1 < n2) ? n1 : n2);
  max = std::to_string((n1 < n2) ? n2 : n1);
  
  block_num++;
  if (block_num == num_blocks) {
    file_num++;
    block_num = 0;
    num_blocks = 700+(rand()%600);
  }
  
  //std::cout<<"\nInserting: ("<<id<<","<<min<<","<<max<<","<<i<<")";
  a.insertInterval(id, min, max, i);

  if (rand() % 10000 < 1) {
    id = std::to_string(rand()%(file_num+1));
    std::cout<<"\nDeleting all intervals starting with: "<<id<<"";
    a.deleteAllIntervals(id);
  }
  
}


}


