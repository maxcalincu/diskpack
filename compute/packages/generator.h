#include "corona.h"
#pragma once

/// Generator API class
class PackingGenerator {

  const std::vector<Interval> radii;                  /// Disk radii
  const BaseType packing_radius;                      /// Determines the size of the generated packing
  std::list<Disk> packing;                            /// Current state of the packing
  QueueType disk_queue;                               /// Queue of disks managed by PackingGenerator::Advance()
  OperatorLookupTable lookup_table;                   /// Tool for efficient disk center computing
  const BaseType precision_upper_bound;               /// Upper bound for disks' interval width 
  std::vector<int> frequency_table;                   /// Used to keep track of disks

  PackingStatus GapFill(Corona &corona);              /// Recursive function which adds disks to the packing while corona is not complete 
  PackingStatus AdvancePacking();                     /// Recursive function which picks a disk from the queue and fills it's corona using GapFill

  void Push(Disk &&new_disk, size_t index);           /// Pushes the disk into the packing and updates the state of the class
  void Pop(size_t index);                             /// Pops the disk from the packing and updates the state of the class. The order is LIFO

  inline bool IsInBounds(const Disk *disk);           /// Determines whether the disk is entirely inside the desired region
  inline bool HasIntersection(const Disk &new_disk);  /// Determines whether new_disk (certainly) intersects any other disk from the packing

public:
  PackingGenerator(const std::vector<Interval> &radii_,
                   const BaseType &packing_radius_,
                   const BaseType &precision_upper_bound_);

  PackingStatus FindPacking();                        /// Starts the generation proccess
  void Dump(const std::string &storage_file);         /// Dumps the packing to a storage once it's completed 
  void Reset();                                       /// Resets the state
};