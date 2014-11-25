
#include "TwoD_IT_w_TopK.h"
#include <algorithm>
#include <functional>


//
TwoD_IT_w_TopK::TwoD_IT_w_TopK() {};

//
TwoD_IT_w_TopK::TwoD_IT_w_TopK(const std::string & filename) {};

//
void TwoD_IT_w_TopK::insertInterval(std::string id, std::string minKey, std::string maxKey, int maxTimestamp) {
storage.push_back(TwoD_Interval(id, minKey, maxKey, maxTimestamp));
};

//
void TwoD_IT_w_TopK::deleteInterval(std::string id) {
storage.remove(TwoD_Interval(id, "", "", 0LL));
};

//
std::vector<TwoD_Interval> TwoD_IT_w_TopK::topK(std::string minKey, std::string maxKey, int k) const {

TwoD_Interval test("test", minKey, maxKey, 0LL);
std::vector<TwoD_Interval> ret_value;

for (std::list<TwoD_Interval>::const_iterator it = storage.begin(); it != storage.end(); it++)
  if ((*it)*test)
    ret_value.push_back(*it);

std::sort(ret_value.begin(), ret_value.end(), std::greater<TwoD_Interval>());
ret_value.erase(ret_value.begin() + k, ret_value.end());

return ret_value;
};

//
void TwoD_IT_w_TopK::sync(const std::string & filename) const {};

