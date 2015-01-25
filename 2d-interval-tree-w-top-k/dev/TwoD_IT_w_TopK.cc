
#include "TwoD_IT_w_TopK.h"
#include <algorithm>
#include <exception>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>



//
static void split(std::list<std::string> &elems, const std::string &s, const char &delim) {
//based on http://stackoverflow.com/a/236803

std::stringstream ss(s);
std::string item1, item2;

std::getline(ss, item1, delim);
elems.push_back(item1);

std::getline(ss, item2);
elems.push_back(item2);
};


//
TwoD_IT_w_TopK::TwoD_IT_w_TopK() {

// set default values
id_delim = '+';
sync_threshold = 10000;
sync_counter = 0;
sync_file = "interval.str";
root = nullptr;
};


//
TwoD_IT_w_TopK::TwoD_IT_w_TopK(const std::string &filename, const bool &sync_from_file) {

// set default values
id_delim = '+';
sync_threshold = 10000;
sync_counter = 0;
root = nullptr;

sync_file = filename;

if (sync_from_file) {
  std::ifstream ifile(filename.c_str());

  if (ifile.is_open()) {
    std::string id, minKey, maxKey;
    uint64_t timestamp;

    while (ifile>>id && ifile>>minKey && ifile>>maxKey && ifile>>timestamp) {
      insertInterval(id, minKey, maxKey, timestamp);
    }
    
    ifile.close();
  }
}
};


//
TwoD_IT_w_TopK::~TwoD_IT_w_TopK() { sync(); };


//
void TwoD_IT_w_TopK::insertInterval(const std::string &id, const std::string &minKey, const std::string &maxKey, const uint64_t &maxTimestamp) {

try {
  TwoD_IT_Node *z = new TwoD_IT_Node, *y = nullptr, *x = root;
  
  std::list<std::string> r;
  split(r, id, id_delim);
  
  if (ids.find(r.front()) == ids.end()) {
    // create empty unordered_set for new key
    ids[r.front()] = std::unordered_set<std::string>();
  }
  else if (ids[r.front()].find(r.back()) != ids[r.front()].end()) {
    // existing id is being rewritten, so delete the old interval from storage
    deleteInterval(id);
  }
  
  ids[r.front()].insert(r.back());
  storage[id]=TwoD_Interval(id, minKey, maxKey, maxTimestamp);
  z->interval = &(storage.find(id)->second);
  storage.find(id)->second.tree_node = z;
  
  while (x != nullptr) {
    y = x;
    if (z->interval->GetLowPoint() < x->interval->GetLowPoint())
      x = x->left;
    else
      x = x->right;
  }
  
  z->parent = y;
  if (y == nullptr)
    root = z;
  else
    if (z->interval->GetLowPoint() < y->interval->GetLowPoint())
      y->left = z;
    else
      y->right = z;
  
  if (++sync_counter > sync_threshold) { sync(); }
}
catch(std::exception &e) {
  std::cerr<<std::endl<<"Insert failure: "<<e.what()<<std::endl;
}
};


//
void TwoD_IT_w_TopK::deleteInterval(const std::string &id) {

if (storage.find(id) != storage.end()) {
  
  std::list<std::string> r;
  split(r, id, id_delim);
  
  ids[r.front()].erase(r.back());
  
  if (ids[r.front()].empty())
    ids.erase(r.front());
  
  TwoD_IT_Node *z = storage.find(id)->second.tree_node, *y, *x;
  
  if (z->left == nullptr || z->right == nullptr)
    y = z;
  else
    y = treeSuccessor(z);
  
  if (y->left != nullptr)
    x = y->left;
  else
    x = y->right;
  
  if (x != nullptr)
    x->parent = y->parent;
  
  if (y->parent == nullptr)
    root = x;
  else
    if (y == y->parent->left)
      y->parent->left = x;
    else
      y->parent->right = x;
  
  if (y != z) {
    z->interval = y->interval;
    z->interval->tree_node = z;
  }
  
  delete y;
  storage.erase(id);

  if (++sync_counter > sync_threshold) { sync(); }
}
};


//
void TwoD_IT_w_TopK::deleteAllIntervals(const std::string &id_prefix) {

if (ids.find(id_prefix) != ids.end()) {
  std::list<std::string> intervals_to_delete;
  
  for (std::unordered_set<std::string>::iterator it = ids[id_prefix].begin(); it != ids[id_prefix].end(); it++) {
    intervals_to_delete.push_back((*it == "") ? id_prefix : id_prefix + id_delim + *it);
  }
  
  for (std::list<std::string>::iterator it = intervals_to_delete.begin(); it != intervals_to_delete.end(); it++) {
    deleteInterval(*it);
  }
}
};


//
void TwoD_IT_w_TopK::getInterval(TwoD_Interval &ret_interval, const std::string &id) const {

std::list<std::string> r;
split(r, id, id_delim);

if (ids.find(r.front()) != ids.end() && ids.at(r.front()).find(r.back()) != ids.at(r.front()).end())
  ret_interval = storage.at(id);
else
  ret_interval = TwoD_Interval("", "", "", 0LL);

};


//
void TwoD_IT_w_TopK::topK(std::vector<TwoD_Interval> &ret_value, const std::string &minKey, const std::string &maxKey, const uint32_t &k) const {

TwoD_Interval test("", minKey, maxKey, 0LL);

for (std::unordered_map<std::string, TwoD_Interval>::const_iterator it = storage.begin(); it != storage.end(); it++) {
  if (it->second * test) {
    ret_value.push_back(it->second);
  }
}

std::sort(ret_value.begin(), ret_value.end(), std::greater<TwoD_Interval>());

if (ret_value.size() > k) {
  ret_value.erase(ret_value.begin() + k, ret_value.end());
}
};


//
void TwoD_IT_w_TopK::printTree() const {

recursivePrintTree(root);
std::cout<<std::endl;
};


//
void TwoD_IT_w_TopK::recursivePrintTree(TwoD_IT_Node* x) const {

if (x != nullptr) {
  recursivePrintTree(x->left);
  std::cout<<" "<<"("<<x->interval->GetId()<<", "<<x->interval->GetLowPoint()<<", "<<x->interval->GetHighPoint()
           <<", "<<x->interval->GetTimeStamp()<<")";
  recursivePrintTree(x->right);
}
};


//
TwoD_IT_Node* TwoD_IT_w_TopK::treeMinimum(TwoD_IT_Node* x) const {

while (x->left != nullptr)
  x = x->left;

return x;
};


//
TwoD_IT_Node* TwoD_IT_w_TopK::treeSuccessor(TwoD_IT_Node* x) const {

if (x->right != nullptr)
  return treeMinimum(x->right);

TwoD_IT_Node* y = x->parent;

while (y != nullptr && x == y->right) {
  x = x->parent;
  y = y->parent;
}

return y;
};


//
void TwoD_IT_w_TopK::sync() const {

std::ofstream ofile(sync_file.c_str());

if (ofile.is_open()) {
  for (std::unordered_map<std::string, TwoD_Interval>::const_iterator it = storage.begin(); it != storage.end(); it++) {
    ofile << it->second.GetId() << std::endl
          << it->second.GetLowPoint() << std::endl
          << it->second.GetHighPoint() << std::endl
          << it->second.GetTimeStamp() << std::endl;
  }

ofile.close();
}

sync_counter = 0;
};


//

void TwoD_IT_w_TopK::setSyncFile(const std::string &filename) { sync_file = filename; };
void TwoD_IT_w_TopK::getSyncFile(std::string &filename) const { filename = sync_file; };

void TwoD_IT_w_TopK::setSyncThreshold(const uint32_t &threshold) { sync_threshold = threshold; };
void TwoD_IT_w_TopK::getSyncThreshold(uint32_t &threshold) const { threshold = sync_threshold; };

void TwoD_IT_w_TopK::setIdDelimiter(const char &delim) { id_delim = delim; };
void TwoD_IT_w_TopK::getIdDelimiter(char &delim) const { delim = id_delim; };

