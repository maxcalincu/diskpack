#include "tools.h"
#include "cw.pb.h"
#include "geometry.h"
#include <boost/numeric/interval/utility_fwd.hpp>

std::random_device rd;
std::mt19937 g(rd());

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

inline size_t OperatorLookUpTable::GetIndex(size_t i, size_t j, size_t k) {
  return i + j * radii.size() + k * radii.size() * radii.size();
}
SpiralSimilarityOperator *
OperatorLookUpTable::operator()(size_t base_t, size_t prev_t, size_t next_t) {
  auto index = GetIndex(base_t, prev_t, next_t);
  if (!presence[index]) {
    values[index] = SpiralSimilarityOperator{
        radii[base_t], radii[prev_t], radii[next_t], base_t, prev_t, next_t};
    presence[index] = true;
  }
  return &values[index];
}
OperatorLookUpTable::OperatorLookUpTable(const std::vector<Interval> &radii_)
    : radii(radii_), identity(),
      values(radii_.size() * radii_.size() * radii_.size()),
      presence(radii_.size() * radii_.size() * radii_.size()){};

SpiralSimilarityOperator *OperatorLookUpTable::operator()() {
  return &identity;
}

inline bool PackingGenerator::HasIntersection(const Disk &new_disk) {
  for (auto &disk : packing) {
    if (new_disk.intersects(disk)) {
      return true;
    }
  }
  return false;
}

SpiralSimilarityOperator
GetOperatorsProduct(const size_t &begin, const size_t &end,
                    const std::vector<SpiralSimilarityOperator *> &operators) {
  switch (end - begin) {
  case 0:
    return SpiralSimilarityOperator();
  case 1:
    return *operators[begin];
  case 2:
    return *operators[begin] * *operators[begin + 1];
  case 3:
    return *operators[begin] * (*operators[begin + 1] * *operators[begin + 2]);
  case 4:
    return (*operators[begin] * *operators[begin + 1]) *
           (*operators[begin + 2] * *operators[begin + 3]);
  default:
    auto mid = (begin + end) / 2;
    return GetOperatorsProduct(begin, mid, operators) *
           GetOperatorsProduct(mid, end, operators);
  }
  throw "Never happens";
}

PackingStatus
PackingGenerator::GapFill(const Disk &base, std::vector<Disk *> &corona,
                          std::vector<SpiralSimilarityOperator *> &operators,
                          IntervalPair &starting_leaf) {
  // std::cout << "gapfill\n";

  std::vector<size_t> shuffle(radii.size());
  for (size_t i = 0; i < radii.size(); ++i) {
    shuffle[i] = i;
  }
  std::shuffle(shuffle.begin(), shuffle.end(), g);
  if (corona.size() > 2) {
    if (corona.back()->intersects(*corona.front())) {
      return PackingStatus::invalid;
    }

    if (corona.back()->tangent(*corona.front())) {
      return AddAnotherDisk();
    }
  }
  for (size_t i = 0; i < radii.size(); ++i) {

    operators.push_back(
        corona.empty()
            ? lut()
            : lut(base.get_type(), corona.back()->get_type(), shuffle[i]));

    starting_leaf =
        (corona.empty()
             ? IntervalPair{base.get_radius() + radii[shuffle[i]], 0.0L}
             : starting_leaf);

    auto new_disk_center =
        GetOperatorsProduct(0, operators.size(), operators) * starting_leaf;

    Disk new_disk(new_disk_center.first + base.get_center_x(),
                  new_disk_center.second + base.get_center_y(),
                  radii[shuffle[i]], shuffle[i]);

    if (HasIntersection(new_disk)) {
      operators.pop_back();
      continue;
    }
    packing.push_back(std::move(new_disk));
    corona.push_back(&packing.back());
    disk_queue.insert(&packing.back());
    ++frequency_table[shuffle[i]];

    // packing.emplace_back(base.get_center_x(), base.get_center_y(),
    // Interval{0.2}, 4);
    auto status = GapFill(base, corona, operators, starting_leaf);
    if (status == PackingStatus::complete) {
      return PackingStatus::complete;
    }
    // packing.pop_back();

    assert(corona.back() == &packing.back());
    operators.pop_back();
    corona.pop_back();
    disk_queue.erase(&packing.back());
    --frequency_table[shuffle[i]];
    packing.pop_back();
  }
  // std::cout << "invalid end of list\n";
  return PackingStatus::invalid;
}

inline bool PackingGenerator::IsInBounds(const Disk *disk) {
  if (disk == nullptr) {
    return false;
  }
  return cerle(disk->get_norm(), packing_radius * packing_radius);
  // return std::max(std::abs(d->get_center_x()), std::abs(d->get_center_y())) <
  // packing_radius;
}

struct DiskClockwiseCompare {
  Interval center_x;
  Interval center_y;
  DiskClockwiseCompare(const Disk &base) {
    center_x = base.get_center_x();
    center_y = base.get_center_y();
  }
  bool operator()(const Disk *a, const Disk *b) const {
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
};

inline void PackingGenerator::GetSortedCorona(const Disk &base,
                                              std::vector<Disk *> &corona) {
  assert(corona.empty());
  for (auto &disk : packing) {
    if (base.tangent(disk)) {
      corona.push_back(&disk);
    }
  }
  std::sort(corona.begin(), corona.end(), DiskClockwiseCompare{base});
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::vector<Disk *> real_corona;
      for (size_t j = 0; j < corona.size(); ++j) {
        real_corona.push_back(corona[(i + j + 1) % corona.size()]);
      }
      corona = std::move(real_corona);
      return;
    }
  }
}

PackingStatus PackingGenerator::AddAnotherDisk() {
  // std::cout << "add\n";
  Disk *base = nullptr;
  while (!IsInBounds(base) && !disk_queue.empty()) {
    base = *(disk_queue.begin());
    disk_queue.erase(disk_queue.begin());
  }
  if (!IsInBounds(base)) {
    return (std::find(frequency_table.begin(), frequency_table.end(), 0) !=
                    frequency_table.end()
                ? PackingStatus::invalid
                : PackingStatus::complete);
  }

  std::vector<SpiralSimilarityOperator *> operators(0);
  std::vector<Disk *> corona(0);
  corona.reserve(12);
  operators.reserve(10);

  GetSortedCorona(*base, corona);
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::cout << "Corona is not continuous!\n";
      packing.emplace_back(base->get_center_x(), base->get_center_y(),
                           Interval{0.2}, 4);
      return PackingStatus::complete;
    }
    // assert(corona[i]->tangent(*corona[i + 1]));
  }
  auto starting_leaf =
      (corona.empty()
           ? std::make_pair(0.0L, 0.0L)
           : std::make_pair(
                 corona.back()->get_center_x() - median(base->get_center_x()),
                 corona.back()->get_center_y() - median(base->get_center_y())));

  auto status = GapFill(*base, corona, operators, starting_leaf);
  if (status == PackingStatus::invalid) {
    disk_queue.insert(base);
  }
  return status;
}
PackingStatus PackingGenerator::FindPacking(const std::string &storage_file) {
  Reset();
  packing.emplace_back(zero, zero, radii[0], 0);
  disk_queue.insert(&packing.back());
  ++frequency_table[0];
  auto status = AddAnotherDisk();
  if (status == PackingStatus::complete) {
    CDP::Packing storage_packing;
    DumpPacking(storage_packing, packing, packing_radius);
    WritePackingToFile(storage_packing, storage_file);
  }
  return status;
}
PackingGenerator::PackingGenerator(const std::vector<Interval> &radii_,
                                   const BaseType &packing_radius_)
    : radii(radii_), packing_radius{packing_radius_}, packing{},
      disk_queue(LessNormCompare), frequency_table(radii_.size(), 0),
      lut(radii_){};

void PackingGenerator::Reset() {
  disk_queue.clear();
  packing.clear();
  for (auto &x : frequency_table) {
    x = 0;
  }
}