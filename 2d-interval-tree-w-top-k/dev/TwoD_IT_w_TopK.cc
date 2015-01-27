
#include "TwoD_IT_w_TopK.h"
#include <algorithm>
#include <exception>
#include <functional>
#include <fstream>
#include <iostream>
#include <list>
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
static std::string max(std::string a, std::string b) {
if (a>b)
  return a;

return b;
};


//
static std::string max(std::string a, std::string b, std::string c) {
if (a>b)
  if (a>c)
    return a;
else
  if (b>c)
    return b;

return c;
};


//
TwoD_IT_w_TopK::TwoD_IT_w_TopK() {

// set default values
id_delim = '+';
sync_threshold = 10000;
sync_counter = 0;
sync_file = "interval.str";
root = &nil;
};


//
TwoD_IT_w_TopK::TwoD_IT_w_TopK(const std::string &filename, const bool &sync_from_file) {

// set default values
id_delim = '+';
sync_threshold = 10000;
sync_counter = 0;
root = &nil;

sync_file = filename;

if (sync_from_file) {
  std::ifstream ifile(filename.c_str());

  if (ifile.is_open()) {
    std::string id, minKey, maxKey;
    uint64_t timestamp;

    while (ifile>>id and ifile>>minKey and ifile>>maxKey and ifile>>timestamp) {
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
  if (id == "")
    throw std::runtime_error("Empty interval ID string");
  
  TwoD_IT_Node *z = new TwoD_IT_Node;
  
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
  storage.find(id)->second.tree_node = z;
  
  z->interval = &(storage.find(id)->second);
  treeInsert(z);
    
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
  
  treeDelete(storage.find(id)->second.tree_node);

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

if (ids.find(r.front()) != ids.end() and ids.at(r.front()).find(r.back()) != ids.at(r.front()).end())
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
void TwoD_IT_w_TopK::treeInsert(TwoD_IT_Node* z) {
TwoD_IT_Node *y = &nil, *x = root;

z->max_high = z->interval->GetHighPoint();
while (x != &nil) {
  y = x;
  if (y->max_high < z->max_high)
    y->max_high = z->max_high;
  if (z->interval->GetLowPoint() < x->interval->GetLowPoint())
    x = x->left;
  else
    x = x->right;
}

z->parent = y;
if (y == &nil)
  root = z;
else
  if (z->interval->GetLowPoint() < y->interval->GetLowPoint())
    y->left = z;
  else
    y->right = z;

z->left = &nil;
z->right = &nil;
z->is_red = true;

treeInsertFixup(z);
};


//
void TwoD_IT_w_TopK::treeInsertFixup(TwoD_IT_Node* z) {
TwoD_IT_Node *y;

while (z->parent->is_red) {
  if (z->parent == z->parent->parent->left) {
    y = z->parent->parent->right;
    if (y->is_red) {
      z->parent->is_red = false;
      y->is_red = false;
      z->parent->parent->is_red = true;
      z = z->parent->parent;
    }
    else {
      if (z == z->parent->right) {
        z = z->parent;
        treeLeftRotate(z);
      }
      z->parent->is_red = false;
      z->parent->parent->is_red = true;
      treeRightRotate(z->parent->parent);
    }
  }
  else {
    y = z->parent->parent->left;
    if (y->is_red) {
      z->parent->is_red = false;
      y->is_red = false;
      z->parent->parent->is_red = true;
      z = z->parent->parent;
    }
    else {
      if (z == z->parent->left) {
        z = z->parent;
        treeRightRotate(z);
      }
      z->parent->is_red = false;
      z->parent->parent->is_red = true;
      treeLeftRotate(z->parent->parent);
    }
  }
}

root->is_red = false;
};


//
void TwoD_IT_w_TopK::treeDelete(TwoD_IT_Node* z) {
TwoD_IT_Node *y, *x, *w;
  
if (z->left == &nil or z->right == &nil)
  y = z;
else
  y = treeSuccessor(z);

if (y->left != &nil)
  x = y->left;
else
  x = y->right;

x->parent = y->parent;

if (y->parent == &nil)
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

w = y->parent;
while (w != &nil) {
  treeSetMaxHigh(w);
  w = w->parent;
}

if (!y->is_red)
  treeDeleteFixup(x);

delete y;
};


//
void TwoD_IT_w_TopK::treeDeleteFixup(TwoD_IT_Node* x) {
TwoD_IT_Node *w;

while (x != root and !x->is_red) {
  if (x == x->parent->left) {
    w = x->parent->right;
    if (w->is_red) {
      w->is_red = false;
      x->parent->is_red = true;
      treeLeftRotate(x->parent);
      w = x->parent->right;
    }
    if (!w->left->is_red and !w->right->is_red) {
      w->is_red = true;
      x = x->parent;
    }
    else {
      if (!w->right->is_red) {
        w->left->is_red = false;
        w->is_red = true;
        treeRightRotate(w);
        w = x->parent->right;
      }
      w->is_red = x->parent->is_red;
      x->parent->is_red = false;
      w->right->is_red = false;
      treeLeftRotate(x->parent);
      x = root;
    }
  }
  else {
    w = x->parent->left;
    if (w->is_red) {
      w->is_red = false;
      x->parent->is_red = true;
      treeRightRotate(x->parent);
      w = x->parent->left;
    }
    if (!w->left->is_red and !w->right->is_red) {
      w->is_red = true;
      x = x->parent;
    }
    else {
      if (!w->left->is_red) {
        w->right->is_red = false;
        w->is_red = true;
        treeLeftRotate(w);
        w = x->parent->left;
      }
      w->is_red = x->parent->is_red;
      x->parent->is_red = false;
      w->left->is_red = false;
      treeRightRotate(x->parent);
      x = root;
    }
  }
}

};


//
void TwoD_IT_w_TopK::recursivePrintTree(TwoD_IT_Node* x) const {

if (x != &nil) {
  recursivePrintTree(x->left);
  std::cout<<" "<<"("<<x->interval->GetId()<<", "<<x->interval->GetLowPoint()<<", "<<x->interval->GetHighPoint()
           <<", "<<x->interval->GetTimeStamp()<<")";
  recursivePrintTree(x->right);
}
};


//
TwoD_IT_Node* TwoD_IT_w_TopK::treeMinimum(TwoD_IT_Node* x) const {

while (x->left != &nil)
  x = x->left;

return x;
};


//
TwoD_IT_Node* TwoD_IT_w_TopK::treeSuccessor(TwoD_IT_Node* x) const {

if (x->right != &nil)
  return treeMinimum(x->right);

TwoD_IT_Node* y = x->parent;

while (y != &nil and x == y->right) {
  x = x->parent;
  y = y->parent;
}

return y;
};


//
void TwoD_IT_w_TopK::treeLeftRotate(TwoD_IT_Node* x) {

TwoD_IT_Node* y = x->right;
x->right = y->left;
y->left->parent = x;
y->parent = x->parent;

if (x->parent == &nil)
  root = y;
else
  if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

y->left= x;
x->parent = y;

treeSetMaxHigh(x);
treeSetMaxHigh(y);
};


//
void TwoD_IT_w_TopK::treeRightRotate(TwoD_IT_Node* x) {

TwoD_IT_Node* y = x->left;
x->left = y->right;
y->right->parent = x;
y->parent = x->parent;

if (x->parent == &nil)
  root = y;
else
  if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;

y->right= x;
x->parent = y;

treeSetMaxHigh(x);
treeSetMaxHigh(y);
};


//
void TwoD_IT_w_TopK::treeSetMaxHigh(TwoD_IT_Node* x) {

if (x->left != &nil)
  if (x->right != &nil)
    x->max_high = max(x->interval->GetHighPoint(), x->left->max_high, x->right->max_high);
  else
    x->max_high = max(x->interval->GetHighPoint(), x->left->max_high);
else
  if (x->right != &nil)
    x->max_high = max(x->interval->GetHighPoint(), x->right->max_high);
  else
    x->max_high = x->interval->GetHighPoint();
};


//
void TwoD_IT_w_TopK::setSyncFile(const std::string &filename) { sync_file = filename; };
void TwoD_IT_w_TopK::getSyncFile(std::string &filename) const { filename = sync_file; };

void TwoD_IT_w_TopK::setSyncThreshold(const uint32_t &threshold) { sync_threshold = threshold; };
void TwoD_IT_w_TopK::getSyncThreshold(uint32_t &threshold) const { threshold = sync_threshold; };

void TwoD_IT_w_TopK::setIdDelimiter(const char &delim) { id_delim = delim; };
void TwoD_IT_w_TopK::getIdDelimiter(char &delim) const { delim = id_delim; };

