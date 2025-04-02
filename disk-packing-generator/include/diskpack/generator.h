#include "diskpack/geometry.h"
#include <diskpack/corona.h>
#pragma once

namespace CDP {

/// Generator API class
class PackingGenerator {

  const std::vector<Interval> radii;          /// Disk radii
  const BaseType precision_upper_bound;       /// Upper bound for disks' interval width
  BaseType packing_radius;                    /// Determines the size of the generated packing
  size_t size_upper_bound;                    /// Upper bound on the number of disks in the packing
  std::list<Disk> packing;                    /// Current state of the packing
  QueueType disk_queue;                       /// Queue of disks managed by PackingGenerator::Advance()
  OperatorLookupTable lookup_table;           /// Tool for efficient disk center computing
  std::vector<int> frequency_table;           /// Used to keep track of disks
  BaseType generated_radius;                  /// The radius of the circle which can be covered
                                              /// by the generated packing

  PackingStatus GapFill(Corona &corona);      /// Recursive function which adds disks to the
                                              /// packing while corona is not complete

  void Push(Disk &&new_disk, size_t index);   /// Pushes the disk into the packing and updates the
                                              /// state of the class
  void Pop(size_t index);                     /// Pops the disk from the packing and updates the
                                              /// state of the class. The order is LIFO

  inline bool
  IsInBounds(const Disk *disk) const;                       /// Determines whether the disk is
                                                            /// entirely inside the desired region
  inline bool HasIntersection(const Disk &new_disk) const;  /// Determines whether new_disk (certainly) intersects any other
                                                            /// disk from the packing

  void SetGeneratedRadius(const Disk &furthest_disk);
  void ShuffleIndexes(std::vector<size_t> &shuffle) const;
  bool SatisfiesConstraints() const;

public:
  PackingStatus AdvancePacking();             /// Recursive function which picks a disk from the queue and
                                              /// fills it's corona using GapFill. Use Generate() to generate
                                              /// packings from scratch or use AdvancePacking to resume generation
                                              /// with a new packing_radius or size_upper_bound
  PackingGenerator(const std::vector<Interval> &radii_,
                   const BaseType &packing_radius_,
                   const BaseType &precision_upper_bound_,
                   const size_t &size_upper_bound_);

  PackingStatus Generate(); /// Starts the generation proccess
  const BaseType &
  GetGeneratedRadius();                                       /// Returns the radius of the generated packing
                                                              /// (which could be less than packing_radius iff
                                                              /// Generate() returns anything other than
                                                              /// PackingStatus::complete)
  
  void SetPackingRadius(const BaseType &new_packing_radius);  /// Set a new value to the desired packing radius
  void SetSizeUpperBound(const size_t &new_size);             /// Set a new value to the desired packing size
  
  const BaseType &GetRadius();                                /// Dumps the packing to a storage once it's completed
  const std::list<Disk> &GetPacking();                        /// Getter function for the generated packing
  void Reset();                                               /// Resets the state
};
} // namespace CDP