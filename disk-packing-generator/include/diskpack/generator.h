#include <diskpack/corona.h>
#include <set>
#include <random>

#pragma once

namespace CDP {

/// Status of generation process
enum PackingStatus {
  complete,         /// packing has been generated
  invalid,          /// no packing with these radii exists
  precision_error,  /// signals that precision upper bound was violated
                    /// (one might need to decrease the packing radius in this case)
  corona_error      /// signals that corona assumption was violated
                    /// (most likely due to the fact that precision upper bound is
                    /// too big)
};

using QueueType = std::multiset<Disk *, decltype(&LessNormCompare)>;    /// Priority queue which supports
                                                                        /// insertion, exctraction on
                                                                        /// key. Yields disks which are
                                                                        /// closer to the (0, 0)
std::ostream &operator<<(std::ostream &out, PackingStatus status);


/// Generator class
class BasicGenerator {
protected:
  std::mt19937 g;

  std::vector<Interval> radii;                /// Disk radii
  const BaseType precision_upper_bound;       /// Upper bound for disks' interval width
  BaseType packing_radius;                    /// Determines the size of the generated packing
  size_t size_upper_bound;                    /// Upper bound on the number of disks in the packing
  std::list<Disk> packing;                    /// Current state of the packing
  QueueType disk_queue;                       /// Queue of disks managed by PackingGenerator::Advance()
  OperatorLookupTable lookup_table;           /// Tool for efficient disk center computing
  std::vector<int> frequency_table;           /// Used to keep track of disks
  BaseType generated_radius;                  /// The radius of the circle which is covered
                                              /// by the generated packing

  PackingStatus GapFill(Corona &corona);      /// Recursive function which adds disks to the
                                              /// packing while corona is not complete

  PackingStatus AdvancePacking();             /// Recursive function which picks a disk from the queue and
                                              /// fills it's corona using GapFill.

  void ShuffleIndexes(std::vector<size_t> &shuffle);  /// Grants randomness to the order of backtracking.
                                                      /// Thus the resulting packing is not determenistic.

  void Push(Disk &&new_disk, size_t index);   /// Pushes the disk into the packing and updates the
                                              /// state of the class
  void Pop(size_t index);                     /// Pops the disk from the packing and updates the
                                              /// state of the class. The order is LIFO

  void SetGeneratedRadius(const Disk &furthest_disk);

  bool HasIntersection(const Disk &new_disk) const;         /// Determines whether new_disk (certainly) intersects any other
                                                            /// disk from the packing

  virtual bool IsInBounds(const Disk *disk) const;          /// Determines whether the disk is
                                                            /// entirely inside the desired region
  virtual bool PackingSatisfiesConstraints() const;         /// Assumptions emposed on the packing are checked.

  virtual bool PackingIsLargeEnough() const;                /// Packing size upper bound checker

  
public:
  BasicGenerator(  const std::vector<Interval> &radii_,
                   const BaseType &packing_radius_,
                   const BaseType &precision_upper_bound_,
                   const size_t &size_upper_bound_);

  PackingStatus Generate(const size_t &central_disk_type = 0);/// Starts the generation proccess
                                                              /// Use Generate() to generate a packing from scratch

  void Reset();                                               /// Resets the state
  PackingStatus Resume();                                     /// Resumes the generation after some hyperparameters have been changed.

  
  void SetPackingRadius(const BaseType &new_packing_radius);  /// Set a new value to the desired packing radius

  void SetSizeUpperBound(const size_t &new_size);             /// Set a new value to the desired packing size
  void SetRadii(const std::vector<Interval> radii_);

  const BaseType &GetGeneratedRadius();                       /// Returns the radius of the generated packing
                                                              /// (which could be less than packing_radius iff
                                                              /// Generate() returns anything other than
                                                              /// PackingStatus::complete)
  const BaseType &GetRadius();                                /// Dumps the packing to a storage once it's completed
  const std::list<Disk> &GetPacking();                        /// Getter function for the generated packing

};

} // namespace CDP