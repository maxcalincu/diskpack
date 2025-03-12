#include "tools.h"

DiskClockwiseCompare::DiskClockwiseCompare(const Disk &base) {
  center_x = base.get_center_x();
  center_y = base.get_center_y();
}
bool DiskClockwiseCompare::operator()(const Disk *a, const Disk *b) const {
  auto ax = a->get_center_x() - center_x;
  auto ay = a->get_center_y() - center_y;
  auto bx = b->get_center_x() - center_x;
  auto by = b->get_center_y() - center_y;
  return (
      cergt(ax, 0.0L) && zero_in(ay)
          ? true
          : (cergt(bx, 0.0L) && zero_in(by)
                 ? false
                 : (cerlt(ay * by, 0.0L) ? cergt(ay, 0.0L)
                                         : cergt(by * ax - ay * bx, 0.0L))));
}

void DumpPacking(CDP::Packing &storage_packing, const std::list<Disk> &packing,
                 BaseType packing_radius) {
  CDP::Disk *disk = nullptr;
  storage_packing.set_dimensions(packing_radius);
  for (auto &d : packing) {
    disk = storage_packing.add_disks();
    disk->set_radius(median(d.get_radius()));
    disk->set_x(median(d.get_center_x()));
    disk->set_y(median(d.get_center_y()));
    disk->set_size_type(d.get_type());
  }
}

void WritePackingToFile(CDP::Packing &packing, const std::string &filename) {
  std::fstream output(filename,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!packing.SerializeToOstream(&output)) {
    std::cerr << "Failed to write packing."
              << "\n";
  }
}

inline size_t OperatorLookupTable::GetIndex(size_t i, size_t j, size_t k) {
  return i + j * radii.size() + k * radii.size() * radii.size();
}
SpiralSimilarityOperator *
OperatorLookupTable::operator()(size_t base_t, size_t prev_t, size_t next_t) {
  auto index = GetIndex(base_t, prev_t, next_t);
  if (!presence[index]) {
    values[index] = SpiralSimilarityOperator{
        radii[base_t], radii[prev_t], radii[next_t], base_t, prev_t, next_t};
    presence[index] = true;
  }
  return &values[index];
}
OperatorLookupTable::OperatorLookupTable(const std::vector<Interval> &radii_)
    : radii(radii_), identity(),
      values(radii_.size() * radii_.size() * radii_.size()),
      presence(radii_.size() * radii_.size() * radii_.size()) {};

SpiralSimilarityOperator *OperatorLookupTable::operator()() {
  return &identity;
}