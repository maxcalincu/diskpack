#include "generator.h"

std::random_device rd;
std::mt19937 g(rd());

inline bool PackingGenerator::HasIntersection(const Disk &new_disk) {
  return std::any_of(
      packing.begin(), packing.end(),
      [&new_disk](const Disk &disk) { return new_disk.intersects(disk); });
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

PackingStatus PackingGenerator::GapFill(Corona &corona) {
  // std::cout << "gapfill\n";

  std::vector<size_t> shuffle(radii.size());
  std::iota(shuffle.begin(), shuffle.end(), 0);
  std::shuffle(shuffle.begin(), shuffle.end(), g);

  if (corona.IsCompleted()) {
    return AdvancePacking();
  }

  Disk new_disk;

  for (size_t i = 0; i < radii.size(); ++i) {
    corona.PeekNewDisk(new_disk, shuffle[i]);
    
    if (new_disk.precision() > precision_threshold) {
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

inline bool PackingGenerator::IsInBounds(const Disk *disk) {
  return disk == nullptr ? false
                         : cerle(disk->get_norm(),
                                 (packing_radius - disk->get_radius()) *
                                     (packing_radius - disk->get_radius()));
}

PackingStatus PackingGenerator::AdvancePacking() {
  // std::cout << "advance\n";
  Disk *base = nullptr;
  while (!IsInBounds(base) && !disk_queue.empty()) {
    base = disk_queue.extract(disk_queue.begin()).value();
  }
  if (!IsInBounds(base)) {
    return (std::find(frequency_table.begin(), frequency_table.end(), 0) !=
                    frequency_table.end()
                ? PackingStatus::invalid
                : PackingStatus::complete);
  }

  Corona corona(*base, packing, lookup_table);
  if (!corona.IsContinuous()) {
    packing.emplace_back(base->get_center_x(), base->get_center_y(),
                         Interval{0.2}, 4);
    return PackingStatus::corona_error;
  }

  auto status = GapFill(corona);
  if (status == PackingStatus::invalid) {
    disk_queue.insert(base);
  }
  return status;
}

PackingStatus PackingGenerator::FindPacking() {
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
                                   const BaseType &precision_threshold_)
    : radii(radii_), packing_radius{packing_radius_},
      disk_queue(LessNormCompare), frequency_table(radii_.size(), 0),
      lookup_table(radii_), precision_threshold(precision_threshold_) {};

void PackingGenerator::Reset() {
  disk_queue.clear();
  packing.clear();
  std::fill(frequency_table.begin(), frequency_table.end(), 0);
}

void PackingGenerator::Dump(const std::string &storage_file) {
  CDP::Packing storage_packing;
  DumpPacking(storage_packing, packing, packing_radius);
  WritePackingToFile(storage_packing, storage_file);
}