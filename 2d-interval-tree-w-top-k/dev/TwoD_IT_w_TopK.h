#ifndef TWOD_IT_W_TOPK_H
#define TWOD_IT_W_TOPK_H

#include <list>
#include <inttypes.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>



class TwoD_IT_Node;

// 1d-interval in interval_dimension-time space
class TwoD_Interval {
public:
  TwoD_Interval() {};
  TwoD_Interval(const std::string id, const std::string low, const std::string high, const uint64_t timestamp) :
    _id(id), _low(low), _high(high), _timestamp(timestamp) {};
  
  std::string GetId() const {return _id;};
  std::string GetLowPoint() const {return _low;};
  std::string GetHighPoint() const {return _high;};
  uint64_t GetTimeStamp() const {return _timestamp;};
  
  bool operator == (const TwoD_Interval& otherInterval)
    const {return (_id == otherInterval._id);}
  bool operator > (const TwoD_Interval& otherInterval)
    const {return (_timestamp > otherInterval._timestamp);}
  
  // overlap operator
  bool operator * (const TwoD_Interval& otherInterval) const {
    // point intersections are considered intersections
    if (_low < otherInterval._low) return (_high >= otherInterval._low);
    return (otherInterval._high >= _low);
    }

  TwoD_IT_Node* tree_node;

protected:
  std::string _id;
  std::string _low;
  std::string _high;
  uint64_t _timestamp;
};


// Interval tree node
class TwoD_IT_Node {
public:
  TwoD_IT_Node() : is_red(false) {};

  TwoD_Interval *interval;
  bool is_red;
  //std::string max_high;
  //uint64_t max_timestamp;
  TwoD_IT_Node *left, *right, *parent;
};


// Storage and index for intervals
class TwoD_IT_w_TopK {
public:
  TwoD_IT_w_TopK();
  TwoD_IT_w_TopK(const std::string &filename, const bool &sync_from_file);
  ~TwoD_IT_w_TopK();

  void insertInterval(const std::string &id, const std::string &minKey, const std::string &maxKey, const uint64_t &maxTimestamp);
  
  void deleteInterval(const std::string &id);
  void deleteAllIntervals(const std::string &id_prefix);
  
  void getInterval(TwoD_Interval &ret_interval, const std::string &id) const;
  void topK(std::vector<TwoD_Interval> &ret_value, const std::string &minKey, const std::string &maxKey, const uint32_t &k) const;
  
  void printTree() const;
  
  void sync() const;

  void setSyncFile(const std::string &filename);
  void getSyncFile(std::string &filename) const;
  void setSyncThreshold(const uint32_t &threshold);
  void getSyncThreshold(uint32_t &threshold) const;
  
  void setIdDelimiter(const char &delim);
  void getIdDelimiter(char &delim) const;

private:
  
  void treeInsert(TwoD_IT_Node* z);
  void treeDelete(TwoD_IT_Node* z);
  void recursivePrintTree(TwoD_IT_Node* x) const;
  TwoD_IT_Node* treeMinimum(TwoD_IT_Node* x) const;
  TwoD_IT_Node* treeSuccessor(TwoD_IT_Node* x) const;
  void treeLeftRotate(TwoD_IT_Node* x);
  void treeRightRotate(TwoD_IT_Node* x);
  
  TwoD_IT_Node *root, nil;
  std::unordered_map<std::string, TwoD_Interval> storage;
  
  std::unordered_map<std::string, std::unordered_set<std::string> > ids;
  char id_delim;
  
  std::string sync_file;
  uint32_t sync_threshold;
  mutable uint32_t sync_counter;
  
};


#endif
