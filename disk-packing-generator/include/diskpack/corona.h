#include <deque>
#include <diskpack/tools.h>
#include <stack>
#pragma once

namespace CDP {

/// Corona class
class Corona {
  const Disk &base; /// The central disk
  std::deque<const Disk *>
      corona; /// Peripheral disks. At any moment two consequitive disks are
              /// tangent. (i. e. disks with indexes i and i + 1 where i + 1 <
              /// corona.size())

  std::vector<SSORef>
      operators_front; /// Helper objects for compuitng new disks' centers
  std::vector<SSORef> operators_back; ///
  IntervalPair leaf_front;            ///
  IntervalPair leaf_back;             ///
  OperatorLookupTable &lookup_table;  ///
  std::stack<bool> push_history;      ///

  SpiralSimilarityOperator
  GetOperatorsProduct( /// Helper function for compuitng new disks' centers
      const size_t &begin, const size_t &end,
      const std::vector<SSORef> &operators) const;

  void GetSortedCorona(
      const std::list<Disk> &packing); /// Collects disks tangent to the base
                                       /// and sorts them anti-clockwise

public:
  Corona(const Disk &b, const std::list<Disk> &packing,
         OperatorLookupTable &lookup_table_);
  bool IsCompleted() const;  /// Checks whether corona is completed
  bool IsContinuous() const; /// Checks the assumption from row 7 and 8
  void PeekNewDisk(Disk &new_disk,
                   size_t index); /// Constructs a new disk without changing the
                                  /// state (eventually)
  void Push(Disk *disk, size_t index); /// Pushes new disk into corona
  void Pop();                          /// Pops a disk in LIFO order
  const Disk &GetBase();
};

} // namespace CDP