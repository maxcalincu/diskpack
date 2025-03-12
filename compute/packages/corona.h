#include "geometry.h"
#include "tools.h"
#include <algorithm>
#include <boost/numeric/interval/utility_fwd.hpp>

class Corona {
  const Disk base;
  std::deque<const Disk *> corona;
  std::vector<SpiralSimilarityOperator *> operators_front;
  std::vector<SpiralSimilarityOperator *> operators_back;
  IntervalPair leaf_front;
  IntervalPair leaf_back;
  OperatorLookupTable &lookup_table;
  std::stack<bool> push_history;

  SpiralSimilarityOperator GetOperatorsProduct(
      const size_t &begin, const size_t &end,
      const std::vector<SpiralSimilarityOperator *> &operators) const;
  void GetSortedCorona(const std::list<Disk> &packing);

public:
  bool IsCompleted() const;
  bool IsContinuous() const;
  Corona(const Disk &b, const std::list<Disk> &packing,
         OperatorLookupTable &lookup_table_);
  void PeekNewDisk(Disk &new_disk, size_t index);
  void Push(Disk *disk, size_t index);
  void Pop();
};