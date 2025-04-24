#include <diskpack/generator.h>
#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdexcept>

std::random_device rd;

namespace diskpack {
  BasicGenerator::BasicGenerator(const std::vector<Interval> &radii_,
                                     const BaseType &packing_radius_,
                                     const BaseType &precision_upper_bound_,
                                     const size_t &size_upper_bound_,
                                     const size_t &max_ignored_radii_)
      : radii(radii_), 
        packing_radius{packing_radius_},
        precision_upper_bound(precision_upper_bound_),
        size_upper_bound(size_upper_bound_),
        max_ignored_radii(max_ignored_radii_),
        disk_queue(LessNormCompare), 
        frequency_table(radii_.size(), 0),
        lookup_table(radii_),
        generated_radius(0),  
        g(rd()),
        graph(lookup_table) {
          if (graph->HasOverflow()) {
            graph.reset();
          }
        };
  
  PackingStatus BasicGenerator::Generate(const size_t &central_disk_type) {
    if (radii.size() < 1) {
      throw std::runtime_error("BasicGenerator::Generate() called with no radii");
    }
    Reset();
    Push(Disk(zero, zero, radii[central_disk_type], central_disk_type), central_disk_type);
  
    for (size_t i = 0; i < radii.size(); ++i) {
      if (i == central_disk_type) {
        continue;
      }
      Push(Disk(radii[central_disk_type] + radii[i], zero, radii[i], i), i);
  
      auto status = AdvancePacking();
      if (status != PackingStatus::invalid) {
        return status;
      }
      Pop(i);
    }
  
    Pop(central_disk_type);
    return PackingStatus::invalid;
  }
  
  PackingStatus BasicGenerator::Resume() {
    if (packing.size() < 2) {
      return PackingStatus::invalid;
    }
    return AdvancePacking();
  }

  void BasicGenerator::Reset() {
    disk_queue.clear();
    packing.clear();
    generated_radius = 0;
    std::fill(frequency_table.begin(), frequency_table.end(), 0);
  }
  
  void BasicGenerator::ShuffleIndexes(std::vector<size_t> &shuffle) {
    std::iota(shuffle.begin(), shuffle.end(), 0);
    std::shuffle(shuffle.begin(), shuffle.end(), g);
  }

  /// Main recursive functions
  PackingStatus BasicGenerator::AdvancePacking() {
    if (disk_queue.empty()) {
      return PackingStatus::invalid;
    }
    
    auto base = disk_queue.extract(disk_queue.begin()).value();
    
    if (base->precision() > precision_upper_bound) {
      SetGeneratedRadius(*base);
      return PackingStatus::precision_error;
    }

    if (!IsInBounds(*base) || PackingIsLargeEnough()) {
      disk_queue.insert(base);
      if (!PackingSatisfiesConstraints()) {
        return PackingStatus::invalid;
      }
      SetGeneratedRadius(*base);
      return PackingStatus::complete;
    }
  
    Corona corona(*base, packing, lookup_table);
    if (!corona.IsContinuous()) {
      SetGeneratedRadius(*base);
      return PackingStatus::corona_error;
    }
  
    auto status = GapFill(corona);
    if (status == PackingStatus::invalid) {
      disk_queue.insert(base);
    }
    return status;
  }

  PackingStatus BasicGenerator::GapFill(Corona &corona) {

    if (corona.IsCompleted()) {
      // corona.DisplaySignature();
      return AdvancePacking();
    }
    
    std::vector<size_t> shuffle(radii.size());
    ShuffleIndexes(shuffle);
  
    Disk new_disk;
  
    for (size_t i = 0; i < radii.size(); ++i) {
    
      if (!corona.PeekNewDisk(new_disk, shuffle[i], graph)) {
        // std::cerr << "wow!!\n";
        continue;
      }
      if (HasIntersection(new_disk)) {
        continue;
      }
  
      Push(std::move(new_disk), shuffle[i]);
      corona.Push(packing.back(), shuffle[i]);
  
      auto status = GapFill(corona);
      if (status != PackingStatus::invalid) {
        return status;
      }
  
      corona.Pop();
      Pop(shuffle[i]);
    }
    return PackingStatus::invalid;
  }
  
  std::ostream &operator<<(std::ostream &out, PackingStatus status) {
    switch (status) {
    case PackingStatus::complete:
      return out << "complete";
    case PackingStatus::invalid:
      return out << "invalid";
    case PackingStatus::corona_error:
      return out << "corona_error";
    case PackingStatus::precision_error:
      return out << "precision_error";
    }
    return out;
  }

  /// Packing stack modifiers

  void BasicGenerator::Push(Disk &&new_disk, size_t index) {
    packing.push_back(std::make_shared<Disk>(std::move(new_disk)));
    disk_queue.insert(packing.back());
    ++frequency_table[index];
  }
  
  void BasicGenerator::Pop(size_t index) {
    --frequency_table[index];
    disk_queue.erase(packing.back());
    packing.pop_back();
  }

  /// Boolean helper functions

  bool BasicGenerator::HasIntersection(const Disk &new_disk) const {
    return std::any_of(
        packing.begin(), packing.end(),
        [&new_disk](const DiskPointer &disk) { return new_disk.intersects(*disk); });
  }

  bool BasicGenerator::IsInBounds(const Disk &disk) const {
    return  cerle(disk.get_norm(),(packing_radius - disk.get_radius()) *
                                        (packing_radius - disk.get_radius()));
  }

  bool BasicGenerator::PackingSatisfiesConstraints() const {
    return std::count(frequency_table.begin(), frequency_table.end(), 0) <= max_ignored_radii;
  }

  bool BasicGenerator::PackingIsLargeEnough() const {
    return packing.size() >= size_upper_bound;
  }

/// Setters

void BasicGenerator::SetGeneratedRadius(const Disk &furthest_disk) {
  auto x = sqrt(furthest_disk.get_norm()) + furthest_disk.get_radius();
  generated_radius =
      median(sqrt(furthest_disk.get_norm()) + furthest_disk.get_radius());
}

void BasicGenerator::SetRadii(const std::vector<Interval> &radii_) {
  Reset();
  radii = radii_;
  lookup_table = std::move(OperatorLookupTable(radii_));
  frequency_table.resize(radii.size(), 0);
}

void BasicGenerator::SetPackingRadius(const BaseType &new_packing_radius) {
  packing_radius = new_packing_radius;
}
void BasicGenerator::SetSizeUpperBound(const size_t &new_size) {
  size_upper_bound = new_size;
}

//Getters

const std::list<DiskPointer> &BasicGenerator::GetPacking() { return packing; }
const BaseType &BasicGenerator::GetRadius() { return packing_radius; }
const BaseType &BasicGenerator::GetGeneratedRadius() { return generated_radius; }

} // namespace CDP