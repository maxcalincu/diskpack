#include "cw.pb.h"
#include "geometry.h"
#pragma once

/// Status of generation process
enum PackingStatus {  complete,         /// packing has been generated 
                      invalid,          /// no packing with these radii exists
                      precision_error,  /// signals that precision upper bound was violated
                                        /// (one might need to decrease the packing radius in this case)
                      corona_error      /// signals that corona assumption was violated 
                                        /// (most likely due to the fact that precision upper bound is too big)
                    };


using QueueType = std::multiset<Disk *, decltype(&LessNormCompare)>;    /// Priority queue which supports insertion, exctraction on key. 
                                                                        /// Yields disks which are closer to the (0, 0)
using SSORef = std::reference_wrapper<SpiralSimilarityOperator>;

/// Lookup table with spiral similarity operators. Since there at most radii.size()^3 different triplets of radii
/// (therefore at most radii.size()^3 different operators) lookup table is used to reduces the number of redundant computations

class OperatorLookupTable {
  std::vector<SpiralSimilarityOperator> values;
  std::vector<bool> presence;
  SpiralSimilarityOperator identity;
  inline size_t GetIndex(size_t i, size_t j, size_t k);

public:
  const std::vector<Interval> &radii;
  SSORef operator()(size_t base_type, size_t prev_type,
                                       size_t next_type);
  SSORef operator()();
  OperatorLookupTable(const std::vector<Interval> &radii_);
};

struct DiskClockwiseCompare {
  Interval center_x;
  Interval center_y;
  DiskClockwiseCompare(const Disk &base);
  bool operator()(const Disk *a, const Disk *b) const;
};

void DumpPacking(CDP::Packing &storage_packing, const std::list<Disk> &packing, /// Helper functions for packing storing
                 BaseType packing_radius);
void WritePackingToFile(CDP::Packing &packing, const std::string &filename);    ///