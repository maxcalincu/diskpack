#include <diskpack/corona.h>
#include <vector>

namespace CDP {

SpiralSimilarityOperator
Corona::GetOperatorsProduct(const size_t &begin, const size_t &end,
                            const std::vector<SSORef> &operators) const {
  switch (end - begin) {
  case 0:
    return SpiralSimilarityOperator();
  case 1:
    return operators[begin];
  case 2:
    return operators[begin].get() * operators[begin + 1].get();
  case 3:
    return operators[begin].get() *
           (operators[begin + 1].get() * operators[begin + 2].get());
  case 4:
    return (operators[begin].get() * operators[begin + 1].get()) *
           (operators[begin + 2].get() * operators[begin + 3].get());
  }
  auto mid = (begin + end) / 2;
  return GetOperatorsProduct(begin, mid, operators) *
         GetOperatorsProduct(mid, end, operators);
}

bool Corona::IsCompleted() const {
  assert(!corona.empty());
  auto cross_product =
      (corona.back()->get_center_x() - base.get_center_x()) *
          (corona.front()->get_center_y() - base.get_center_y()) -
      (corona.front()->get_center_x() - base.get_center_x()) *
          (corona.back()->get_center_y() - base.get_center_y());

  return (corona.size() > 2 && corona.back()->tangent(*corona.front()) &&
          cergt(cross_product, 0.0L));
}

void Corona::GetSortedCorona(const std::list<Disk> &packing) {
  assert(corona.empty());
  for (auto &disk : packing) {
    if (base.tangent(disk)) {
      corona.push_back(&disk);
    }
  }
  std::sort(corona.begin(), corona.end(), DiskClockwiseCompare{base});
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      std::rotate(corona.begin(), corona.begin() + i + 1, corona.end());
      return;
    }
  }
}

bool Corona::IsContinuous() const {
  for (size_t i = 0; i + 1 < corona.size(); ++i) {
    if (!corona[i]->tangent(*corona[i + 1])) {
      return false;
    }
  }
  return true;
}

Corona::Corona(const Disk &b, const std::list<Disk> &packing,
               OperatorLookupTable &lookup_table_)
    : base(b), lookup_table(lookup_table_) {
  operators_back.reserve(12);
  operators_front.reserve(12);

  GetSortedCorona(packing);
  assert(!corona.empty());
  leaf_front =
      IntervalPair{corona.front()->get_center_x() - base.get_center_x(),
                   corona.front()->get_center_y() - base.get_center_y()};

  leaf_back = IntervalPair{corona.back()->get_center_x() - base.get_center_x(),
                           corona.back()->get_center_y() - base.get_center_y()};
}

void Corona::PeekNewDisk(Disk &new_disk, size_t index) {
  bool use_front = corona.front()->precision() < corona.back()->precision();

  auto &operators = (use_front) ? operators_front : operators_back;
  auto &starting_leaf = (use_front) ? leaf_front : leaf_back;

  operators.push_back(lookup_table(base.get_type(),
                                   use_front ? corona.front()->get_type()
                                             : corona.back()->get_type(),
                                   index));

  auto op = GetOperatorsProduct(0, operators.size(), operators);
  op.y *= (use_front ? -1 : 1);
  auto new_disk_center = op * starting_leaf;

  new_disk = Disk(new_disk_center.first + base.get_center_x(),
                  new_disk_center.second + base.get_center_y(),
                  lookup_table.radii[index], index);
  operators.pop_back();
}
void Corona::Push(Disk *disk, size_t index) {
  bool use_front = corona.front()->precision() < corona.back()->precision();
  push_history.push(use_front);
  auto &operators = (use_front) ? operators_front : operators_back;
  operators.push_back(lookup_table(base.get_type(),
                                   use_front ? corona.front()->get_type()
                                             : corona.back()->get_type(),
                                   index));
  (use_front) ? corona.push_front(disk) : corona.push_back(disk);
}

void Corona::Pop() {
  auto use_front = push_history.top();
  push_history.pop();
  (use_front) ? operators_front.pop_back() : operators_back.pop_back();

  (use_front) ? corona.pop_front() : corona.pop_back();
}

const Disk &Corona::GetBase() { return base; }

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

inline size_t OperatorLookupTable::GetIndex(size_t i, size_t j, size_t k) {
  return i + j * radii.size() + k * radii.size() * radii.size();
}
SSORef OperatorLookupTable::operator()(size_t base_t, size_t prev_t,
                                       size_t next_t) {
  auto index = GetIndex(base_t, prev_t, next_t);
  if (!presence[index]) {
    values[index] = SpiralSimilarityOperator{
        radii[base_t], radii[prev_t], radii[next_t], base_t, prev_t, next_t};
    presence[index] = true;
  }
  return std::ref(values[index]);
}
OperatorLookupTable::OperatorLookupTable(const std::vector<Interval> &radii_)
    : radii(radii_), identity(),
      values(radii_.size() * radii_.size() * radii_.size()),
      presence(radii_.size() * radii_.size() * radii_.size()) {};

SSORef OperatorLookupTable::operator()() { return std::ref(identity); }


} // namespace CDP