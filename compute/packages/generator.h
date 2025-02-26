#include "geometry.h"
#include "tools.h"
#include <algorithm>
#include <boost/numeric/interval/utility_fwd.hpp>

#pragma once

class PackingGenerator {

  const std::vector<Interval> radii;
  const BaseType packing_radius;
  std::list<Disk> packing;
  QueueType disk_queue;
  OperatorLookUpTable lut;

  std::vector<int> frequency_table;

  PackingStatus
  GapFill(const Disk &base, std::deque<Disk *> &corona,
          std::vector<SpiralSimilarityOperator *> &operators_front,
          IntervalPair &leaf_front,
          std::vector<SpiralSimilarityOperator *> &operators_back,
          IntervalPair &leaf_back);
  PackingStatus AdvancePacking();
  inline void GetSortedCorona(const Disk &base, std::deque<Disk *> &corona);
  bool IsCoronaContinuous(const Disk *base, const std::deque<Disk *> &corona);

public:
  PackingGenerator(const std::vector<Interval> &radii_,
                   const BaseType &packing_radius_);
  PackingStatus FindPacking();
  void Dump(const std::string &storage_file);
  void Reset();
  inline bool IsInBounds(const Disk *disk);
  inline bool HasIntersection(const Disk &new_disk);
};