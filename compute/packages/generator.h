#include "corona.h"
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
  OperatorLookupTable lookup_table;
  const BaseType precision_threshold;

  std::vector<int> frequency_table;

  PackingStatus GapFill(Corona &corona);
  PackingStatus AdvancePacking();

  void Push(Disk &&new_disk, size_t index);
  void Pop(size_t index);

public:
  PackingGenerator(const std::vector<Interval> &radii_,
                   const BaseType &packing_radius_,
                   const BaseType &precision_threshold_);
  PackingStatus FindPacking();
  void Dump(const std::string &storage_file);
  void Reset();
  inline bool IsInBounds(const Disk *disk);
  inline bool HasIntersection(const Disk &new_disk);
};