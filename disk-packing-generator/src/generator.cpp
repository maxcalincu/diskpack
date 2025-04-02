#include "diskpack/tools.h"
#include <diskpack/generator.h>
#include <random>

std::random_device rd;
std::mt19937 g(rd());

namespace CDP {
inline bool PackingGenerator::HasIntersection(const Disk &new_disk) const {
  return std::any_of(
      packing.begin(), packing.end(),
      [&new_disk](const Disk &disk) { return new_disk.intersects(disk); });
}

inline bool PackingGenerator::IsInBounds(const Disk *disk) const {
  return disk == nullptr ? false
                         : cerle(disk->get_norm(),
                                 (packing_radius - disk->get_radius()) *
                                     (packing_radius - disk->get_radius()));
}

void PackingGenerator::SetGeneratedRadius(const Disk &furthest_disk) {
  generated_radius =
      median(sqrt(furthest_disk.get_norm()) + furthest_disk.get_radius());
}

void PackingGenerator::SetPackingRadius(const BaseType &new_packing_radius) {
  packing_radius = new_packing_radius;
}
void PackingGenerator::SetSizeUpperBound(const size_t &new_size) {
  size_upper_bound = new_size;
}

void PackingGenerator::Push(Disk &&new_disk, size_t index) {
  packing.push_back(std::move(new_disk));
  disk_queue.insert(&packing.back());
  ++frequency_table[index];
}

void PackingGenerator::Pop(size_t index) {
  disk_queue.erase(&packing.back());
  --frequency_table[index];
  packing.pop_back();
}

void PackingGenerator::ShuffleIndexes(std::vector<size_t> &shuffle) const {
  std::iota(shuffle.begin(), shuffle.end(), 0);
  std::shuffle(shuffle.begin(), shuffle.end(), g);
}

PackingStatus PackingGenerator::GapFill(Corona &corona) {

  std::vector<size_t> shuffle(radii.size());
  ShuffleIndexes(shuffle);

  if (corona.IsCompleted()) {
    return AdvancePacking();
  }

  Disk new_disk;

  for (size_t i = 0; i < radii.size(); ++i) {
    corona.PeekNewDisk(new_disk, shuffle[i]);

    if (new_disk.precision() > precision_upper_bound) {
      SetGeneratedRadius(corona.GetBase());
      return PackingStatus::precision_error;
    }

    if (HasIntersection(new_disk)) {
      continue;
    }

    Push(std::move(new_disk), shuffle[i]);
    corona.Push(&packing.back(), shuffle[i]);

    auto status = GapFill(corona);
    if (status != PackingStatus::invalid) {
      return status;
    }

    corona.Pop();
    Pop(shuffle[i]);
  }
  return PackingStatus::invalid;
}

bool PackingGenerator::SatisfiesConstraints() const {
  return std::find(frequency_table.begin(), frequency_table.end(), 0) == frequency_table.end();
}

PackingStatus PackingGenerator::AdvancePacking() {
  Disk *base = nullptr;
  if (disk_queue.empty()) {
    return PackingStatus::invalid;
  }
  
  base = disk_queue.extract(disk_queue.begin()).value();

  if (!IsInBounds(base)) {
    disk_queue.insert(base);
    if (!SatisfiesConstraints()) {
      return PackingStatus::invalid;
    }
    SetGeneratedRadius(*base);
    return PackingStatus::complete;
  }

  if (packing.size() >= size_upper_bound) {
    SetGeneratedRadius(*base);
    return PackingStatus::complete;
  }

  Corona corona(*base, packing, lookup_table);
  if (!corona.IsContinuous()) {
    // packing.emplace_back(base->get_center_x(), base->get_center_y(),
    // Interval{0.2}, 4);
    SetGeneratedRadius(*base);
    return PackingStatus::corona_error;
  }

  auto status = GapFill(corona);
  if (status == PackingStatus::invalid) {
    disk_queue.insert(base);
  }
  return status;
}

PackingStatus PackingGenerator::Generate() {
  Reset();
  Push(Disk(zero, zero, radii[0], 0), 0);

  for (size_t i = 1; i < radii.size(); ++i) {
    Push(Disk(radii[0] + radii[i], zero, radii[i], i), i);

    auto status = AdvancePacking();
    if (status != PackingStatus::invalid) {
      return status;
    }
    Pop(i);
  }

  Pop(0);
  return PackingStatus::invalid;
}

PackingGenerator::PackingGenerator(const std::vector<Interval> &radii_,
                                   const BaseType &packing_radius_,
                                   const BaseType &precision_upper_bound_,
                                   const size_t &size_upper_bound_)
    : radii(radii_), packing_radius{packing_radius_},
      disk_queue(LessNormCompare), frequency_table(radii_.size(), 0),
      lookup_table(radii_), precision_upper_bound(precision_upper_bound_),
      generated_radius(0), size_upper_bound(size_upper_bound_){};

void PackingGenerator::Reset() {
  disk_queue.clear();
  packing.clear();
  generated_radius = 0;
  std::fill(frequency_table.begin(), frequency_table.end(), 0);
}

const std::list<Disk> &PackingGenerator::GetPacking() { return packing; }
const BaseType &PackingGenerator::GetRadius() { return packing_radius; }
const BaseType &PackingGenerator::GetGeneratedRadius() { return generated_radius; }

} // namespace CDP