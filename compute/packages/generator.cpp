#include "generator.h"
#include "geometry.h"
#include "tools.h"

std::random_device rd;
std::mt19937 g(rd());

inline bool PackingGenerator::HasIntersection(const Disk &new_disk) {

  return std::any_of(
      packing.begin(), packing.end(),
      [&new_disk](const Disk &disk) { return new_disk.intersects(disk); });
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

PackingStatus PackingGenerator::GapFill(
    const Disk &base, std::deque<Disk *> &corona,
    std::vector<SpiralSimilarityOperator *> &operators_front,
    IntervalPair &leaf_front,
    std::vector<SpiralSimilarityOperator *> &operators_back,
    IntervalPair &leaf_back) {
  // std::cout << "gapfill\n";

  std::vector<size_t> shuffle(radii.size());
  std::iota(shuffle.begin(), shuffle.end(), 0);
  std::shuffle(shuffle.begin(), shuffle.end(), g);
  if (corona.size() > 2) {
    if (corona.back()->intersects(*corona.front())) {
      return PackingStatus::invalid;
    }

    if (corona.back()->tangent(*corona.front())) {
      return AdvancePacking();
    }
  }
  bool use_front =
      corona.empty() ? false
                     : corona.front()->precision() < corona.back()->precision();
  auto &operators = (use_front) ? operators_front : operators_back;
  auto &starting_leaf = (use_front) ? leaf_front : leaf_back;

  for (size_t i = 0; i < radii.size(); ++i) {
    operators.push_back(corona.empty()
                            ? lut()
                            : lut(base.get_type(),
                                  use_front ? corona.front()->get_type()
                                            : corona.back()->get_type(),
                                  shuffle[i]));

    leaf_front =
        (corona.empty()
             ? IntervalPair{base.get_radius() + radii[shuffle[i]], 0.0L}
             : leaf_front);
    leaf_back = (corona.empty()
                     ? IntervalPair{base.get_radius() + radii[shuffle[i]], 0.0L}
                     : leaf_back);

    auto op = GetOperatorsProduct(0, operators.size(), operators);
    op.y *= (use_front ? -1 : 1);
    auto new_disk_center = op * starting_leaf;

    Disk new_disk(new_disk_center.first + base.get_center_x(),
                  new_disk_center.second + base.get_center_y(),
                  radii[shuffle[i]], shuffle[i]);

    if (HasIntersection(new_disk)) {
      operators.pop_back();
      continue;
    }
    packing.push_back(std::move(new_disk));
    (use_front) ? corona.push_front(&packing.back())
                : corona.push_back(&packing.back());
    disk_queue.insert(&packing.back());
    ++frequency_table[shuffle[i]];

    auto status = GapFill(base, corona, operators_front, leaf_front,
                          operators_back, leaf_back);
    if (status == PackingStatus::complete) {
      return PackingStatus::complete;
    }

    operators.pop_back();
    (use_front) ? corona.pop_front() : corona.pop_back();
    disk_queue.erase(&packing.back());
    --frequency_table[shuffle[i]];
    packing.pop_back();
  }
  return PackingStatus::invalid;
}

inline bool PackingGenerator::IsInBounds(const Disk *disk) {
  return disk == nullptr ? false
                         : cerle(disk->get_norm(),
                                 (packing_radius - disk->get_radius()) *
                                     (packing_radius - disk->get_radius()));
}

inline void PackingGenerator::GetSortedCorona(const Disk &base,
                                              std::deque<Disk *> &corona) {
  assert(corona.empty());
  for (auto &disk : packing) {
    if (base.tangent(disk)) {
      corona.push_back(&disk);
    }
  }
  std::sort(corona.begin(), corona.end(), DiskClockwiseCompare{base});
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::deque<Disk *> real_corona;
      for (size_t j = 0; j < corona.size(); ++j) {
        real_corona.push_back(corona[(i + j + 1) % corona.size()]);
      }
      corona = std::move(real_corona);
      return;
    }
  }
}

bool PackingGenerator::IsCoronaContinuous(const Disk *base,
                                          const std::deque<Disk *> &corona) {
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::cout << "Corona is not continuous!\n";
      packing.emplace_back(base->get_center_x(), base->get_center_y(),
                           Interval{0.2}, 4);
      return false;
    }
    // assert(corona[i]->tangent(*corona[i + 1]));
  }
  return true;
}

PackingStatus PackingGenerator::AdvancePacking() {
  // std::cout << "advance\n";
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

  std::vector<SpiralSimilarityOperator *> operators_back(0), operators_front(0);
  operators_back.reserve(10);
  operators_front.reserve(10);

  std::deque<Disk *> corona(0);
  GetSortedCorona(*base, corona);
  if (!IsCoronaContinuous(base, corona)) {
    return PackingStatus::complete;
  }
  auto leaf_front =
      (corona.empty()
           ? std::make_pair(0.0L, 0.0L)
           : std::make_pair(
                 corona.front()->get_center_x() - base->get_center_x(),
                 corona.front()->get_center_y() - base->get_center_y()));
  auto leaf_back =
      (corona.empty()
           ? std::make_pair(0.0L, 0.0L)
           : std::make_pair(
                 corona.back()->get_center_x() - base->get_center_x(),
                 corona.back()->get_center_y() - base->get_center_y()));

  auto status = GapFill(*base, corona, operators_front, leaf_front,
                        operators_back, leaf_back);
  if (status == PackingStatus::invalid) {
    disk_queue.insert(base);
  }
  return status;
}

PackingStatus PackingGenerator::FindPacking() {
  Reset();
  packing.emplace_back(zero, zero, radii[0], 0);
  disk_queue.insert(&packing.back());
  ++frequency_table[0];
  return AdvancePacking();
}

PackingGenerator::PackingGenerator(const std::vector<Interval> &radii_,
                                   const BaseType &packing_radius_)
    : radii(radii_), packing_radius{packing_radius_}, packing{},
      disk_queue(LessNormCompare), frequency_table(radii_.size(), 0),
      lut(radii_) {};

void PackingGenerator::Reset() {
  disk_queue.clear();
  packing.clear();
  for (auto &x : frequency_table) {
    x = 0;
  }
}

void PackingGenerator::Dump(const std::string &storage_file) {
  CDP::Packing storage_packing;
  DumpPacking(storage_packing, packing, packing_radius);
  WritePackingToFile(storage_packing, storage_file);
}