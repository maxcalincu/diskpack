#include <diskpack/geometry.h>
#include <stack>
#include <vector>
#include <list>

#pragma once

namespace diskpack {
using SSORef = std::reference_wrapper<SpiralSimilarityOperator>;
  /// Lookup table with spiral similarity operators. Since there at most
  /// radii.size()^3 different triplets of radii (therefore at most radii.size()^3
  /// different operators) lookup table is used to reduces the number of redundant
  /// computations
  class OperatorLookupTable {
    std::vector<SpiralSimilarityOperator> values;
    std::vector<bool> presence;
    SpiralSimilarityOperator identity;
    inline size_t GetIndex(size_t i, size_t j, size_t k);
  
  public:
    std::vector<Interval> radii;
    SSORef operator()(size_t base_type, size_t prev_type, size_t next_type);
    SSORef operator()();
    OperatorLookupTable(const std::vector<Interval> &radii_);
  };
  
  struct DiskClockwiseCompare {
    Interval center_x;
    Interval center_y;
    DiskClockwiseCompare(const Disk &base);
    bool operator()(const DiskPointer a, const DiskPointer b) const;
  };

/// Corona class
class Corona {

  const Disk &base;                 /// The central disk
  std::deque<DiskPointer> corona;   /// Peripheral disks. At any moment two consequitive disks are
                                    /// tangent. (i. e. disks with indexes i and i + 1 where i + 1 <
                                    /// corona.size())

  std::vector<SSORef> operators_front;  /// Helper objects for compuitng new disks' centers
  std::vector<SSORef> operators_back;   ///
  IntervalPair leaf_front;              ///
  IntervalPair leaf_back;               ///
  OperatorLookupTable &lookup_table;    ///
  std::stack<bool> push_history;        ///

  SpiralSimilarityOperator
  GetOperatorsProduct(                 /// Helper function for compuitng new disks' centers
      const size_t &begin, const size_t &end,
      const std::vector<SSORef> &operators) const;

  void GetSortedCorona(
      const std::list<DiskPointer> &packing); /// Collects disks tangent to the base
                                       /// and sorts them anti-clockwise

public:
  Corona(const Disk &b, const std::list<DiskPointer> &packing,
         OperatorLookupTable &lookup_table_);
  bool IsCompleted();            /// Checks whether corona is completed
  bool IsContinuous() const;           /// Checks the assumption from row 7 and 8
  void PeekNewDisk(Disk &new_disk,
                   size_t index);      /// Constructs a new disk without changing the
                                       /// state (eventually)
  void Push(DiskPointer disk, size_t index); /// Pushes new disk into corona
  void Pop();                          /// Pops a disk in LIFO order
  const Disk &GetBase();
};

} // namespace CDP