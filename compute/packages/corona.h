#include "geometry.h"
#include "tools.h"
#include <algorithm>
#include <boost/numeric/interval/utility_fwd.hpp>
#pragma once

class Corona {
  const Disk &base;
  std::deque<const Disk *> corona;
  std::vector<SSORef> operators_front;
  std::vector<SSORef> operators_back;
  IntervalPair leaf_front;
  IntervalPair leaf_back;
  OperatorLookupTable &lookup_table;
  std::stack<bool> push_history;

  SpiralSimilarityOperator GetOperatorsProduct(
      const size_t &begin, const size_t &end,
      const std::vector<SSORef> &operators) const;
  void GetSortedCorona(const std::list<Disk> &packing);

public:
  Corona(const Disk &b, const std::list<Disk> &packing,
    OperatorLookupTable &lookup_table_);
  bool IsCompleted() const;
  bool IsContinuous() const;
  void PeekNewDisk(Disk &new_disk, size_t index);
  void Push(Disk *disk, size_t index);
  void Pop();
};