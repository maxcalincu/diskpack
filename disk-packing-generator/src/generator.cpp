#include <diskpack/generator.h>
#include <../generated/cw.pb.h>
#include <fstream>
#include <algorithm>
#include <random>

std::random_device rd;
std::mt19937 g(rd());

namespace CDP {
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
  
  void DumpPacking(const std::string &storage_file,
                   const std::list<Disk> &packing, BaseType packing_radius) {
    ProtoPacking storage_packing;
    ProtoDisk *disk = nullptr;
    storage_packing.set_dimensions(packing_radius);
  
    for (auto &d : packing) {
      disk = storage_packing.add_disks();
      disk->set_radius(median(d.get_radius()));
      disk->set_x(median(d.get_center_x()));
      disk->set_y(median(d.get_center_y()));
      disk->set_size_type(d.get_type());
    }
  
    std::fstream output(storage_file,
                        std::ios::out | std::ios::trunc | std::ios::binary);
    if (!storage_packing.SerializeToOstream(&output)) {
      std::cerr << "Failed to write packing\n";
    }
  }

bool BasicGenerator::HasIntersection(const Disk &new_disk) const {
  return std::any_of(
      packing.begin(), packing.end(),
      [&new_disk](const Disk &disk) { return new_disk.intersects(disk); });
}

bool BasicGenerator::IsInBounds(const Disk *disk) const {
  return disk == nullptr ? false
                         : cerle(disk->get_norm(),
                                 (packing_radius - disk->get_radius()) *
                                     (packing_radius - disk->get_radius()));
}

void BasicGenerator::SetGeneratedRadius(const Disk &furthest_disk) {
  generated_radius =
      median(sqrt(furthest_disk.get_norm()) + furthest_disk.get_radius());
}

void BasicGenerator::SetPackingRadius(const BaseType &new_packing_radius) {
  packing_radius = new_packing_radius;
}
void BasicGenerator::SetSizeUpperBound(const size_t &new_size) {
  size_upper_bound = new_size;
}

void BasicGenerator::Push(Disk &&new_disk, size_t index) {
  packing.push_back(std::move(new_disk));
  disk_queue.insert(&packing.back());
  ++frequency_table[index];
}

void BasicGenerator::Pop(size_t index) {
  disk_queue.erase(&packing.back());
  --frequency_table[index];
  packing.pop_back();
}

void BasicGenerator::ShuffleIndexes(std::vector<size_t> &shuffle) const {
  std::iota(shuffle.begin(), shuffle.end(), 0);
  std::shuffle(shuffle.begin(), shuffle.end(), g);
}

PackingStatus BasicGenerator::GapFill(Corona &corona) {

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

bool BasicGenerator::SatisfiesConstraints() const {
  return std::find(frequency_table.begin(), frequency_table.end(), 0) == frequency_table.end();
}

bool BasicGenerator::IsLargeEnough() const {
  return packing.size() >= size_upper_bound;
}

PackingStatus BasicGenerator::AdvancePacking() {
  Disk *base = nullptr;
  if (disk_queue.empty()) {
    return PackingStatus::invalid;
  }
  
  base = disk_queue.extract(disk_queue.begin()).value();

  if (!IsInBounds(base) || IsLargeEnough()) {
    disk_queue.insert(base);
    if (!SatisfiesConstraints()) {
      return PackingStatus::invalid;
    }
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

PackingStatus BasicGenerator::Generate(const size_t &central_disk_type) {
  if (radii.size() < 1) {
    return PackingStatus::invalid;
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

BasicGenerator::BasicGenerator(const std::vector<Interval> &radii_,
                                   const BaseType &packing_radius_,
                                   const BaseType &precision_upper_bound_,
                                   const size_t &size_upper_bound_)
    : radii(radii_), packing_radius{packing_radius_},
      disk_queue(LessNormCompare), frequency_table(radii_.size(), 0),
      lookup_table(radii_), precision_upper_bound(precision_upper_bound_),
      generated_radius(0), size_upper_bound(size_upper_bound_){};

void BasicGenerator::Reset() {
  disk_queue.clear();
  packing.clear();
  generated_radius = 0;
  std::fill(frequency_table.begin(), frequency_table.end(), 0);
}

PackingStatus BasicGenerator::Resume() {
  if (packing.size() < 2) {
    return PackingStatus::invalid;
  }
  return AdvancePacking();
}

const std::list<Disk> &BasicGenerator::GetPacking() { return packing; }
const BaseType &BasicGenerator::GetRadius() { return packing_radius; }
const BaseType &BasicGenerator::GetGeneratedRadius() { return generated_radius; }

} // namespace CDP