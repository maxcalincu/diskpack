#include "cw.pb.h"
#include "geometry.h"
#pragma once

enum PackingStatus { complete, invalid, precision_error, corona_error };

typedef std::multiset<Disk *, decltype(&LessNormCompare)> QueueType;
typedef std::vector<const SpiralSimilarityOperator *> OperatorCollection;

class OperatorLookupTable {
  std::vector<SpiralSimilarityOperator> values;
  std::vector<bool> presence;
  SpiralSimilarityOperator identity;
  inline size_t GetIndex(size_t i, size_t j, size_t k);

public:
  const std::vector<Interval> &radii;
  SpiralSimilarityOperator *operator()(size_t base_type, size_t prev_type,
                                       size_t next_type);
  SpiralSimilarityOperator *operator()();
  OperatorLookupTable(const std::vector<Interval> &radii_);
};

struct DiskClockwiseCompare {
  Interval center_x;
  Interval center_y;
  DiskClockwiseCompare(const Disk &base);
  bool operator()(const Disk *a, const Disk *b) const;
};

void DumpPacking(CDP::Packing &storage_packing, const std::list<Disk> &packing,
                 BaseType packing_radius);
void WritePackingToFile(CDP::Packing &packing, const std::string &filename);