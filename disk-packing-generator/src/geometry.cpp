#include <diskpack/geometry.h>

namespace diskpack {
 /// RadiiRegion
 void RadiiRegion::Split(std::vector<RadiiRegion> &regions, size_t k, std::optional<size_t> index) const {
  regions.clear();
  regions.reserve(k);
  auto intervals_copy = intervals;
  auto it = !index.has_value() ? std::max_element(intervals_copy.begin(), intervals_copy.end(), [](const Interval& a, const Interval& b) {
      return width(a) < width(b);
  }) : intervals_copy.begin() + index.value();

  auto initial = *it;
  auto sub_width = (initial.upper() - initial.lower())/k;
  auto next = std::next(it);
  for (size_t i = 0; i < k; ++i) {
      auto l =              initial.lower() + (i + 0) * sub_width;
      auto u = (i < k - 1 ? initial.lower() + (i + 1) * sub_width : initial.upper());
      *it = Interval{l, u};
      if (next != intervals_copy.end()) {
        if (cergt(*it, *next)) {
          continue;
        }
      }
      if (it != intervals_copy.begin()) {
        if (cergt(*std::prev(it), *it)) {
          continue;
        }
      }
      regions.emplace_back(intervals_copy);
  }
}
void RadiiRegion::GridSplit(std::vector<RadiiRegion> &regions, size_t k, size_t index) const {
if (index == 0) {
  regions.clear();
  size_t s = 1;
  for (size_t i = 0; i + 1 < intervals.size(); ++i) {
    s *= k;
  }
  regions.reserve(s);
}
if (index + 1 >= intervals.size()) {
  regions.emplace_back(intervals);
  return;
}
std::vector<RadiiRegion> children_regions;
Split(children_regions, k, {});
for (auto &cr : children_regions) {
  cr.GridSplit(regions, k, index + 1);
}
}
const std::vector<Interval>& RadiiRegion::GetIntervals() const { return intervals; }
bool RadiiRegion::IsNarrowEnough(BaseType lower_bound) const {
  return !std::any_of(intervals.begin(), intervals.end(), [&lower_bound](const Interval& x) {
      return width(x) > lower_bound;
  }); 
}

bool RadiiRegion::IsTooWide(BaseType upper_bound) const {
  return std::any_of(intervals.begin(), intervals.end(), [&upper_bound](const Interval& x) {
      return width(x) > upper_bound;
  });
}

RadiiRegion::RadiiRegion(const std::vector<Interval> &intervals_): intervals(intervals_) {};
RadiiRegion::RadiiRegion(std::vector<Interval> &&intervals_): intervals(std::move(intervals_)) {}
Interval RadiiRegion::GetMinInterval() const {
return intervals.front();
}
Interval RadiiRegion::GetMaxInterval() const {
return intervals.back();
}

bool RadiiCompare::operator()(const std::vector<Interval> &a, const std::vector<Interval> &b) const {
  for (size_t i = 0; i < a.size(); ++i) {
      if (a[i].lower() != b[i].lower()) {
          return a[i].lower() < b[i].lower();
      }
      if (a[i].upper() != b[i].upper()) {
          return a[i].upper() < b[i].upper();
      }
  }
  return false;
}

//SpiralSimilarityOperator

SpiralSimilarityOperator::SpiralSimilarityOperator(const Interval &x_,
                                                   const Interval &y_)
    : x{x_}, y{y_} {}
SpiralSimilarityOperator::SpiralSimilarityOperator(
    const SpiralSimilarityOperator &other)
    : x{other.x}, y{other.y} {}
SpiralSimilarityOperator::SpiralSimilarityOperator(
    SpiralSimilarityOperator &&other)
    : x{std::move(other.x)}, y{std::move(other.y)} {}

SpiralSimilarityOperator::SpiralSimilarityOperator(
    const Interval &base_r, const Interval &prev_r, const Interval &next_r,
    const size_t &base_t, const size_t &prev_t, const size_t &next_t) {

  const auto &b_r = base_r, &p_r = prev_r, &n_r = next_r;
  const auto &b_t = base_t, &p_t = prev_t, &n_t = next_t;

  // x = ((square(n_r + p_r) - square(n_r + p_r))/square(n_r + b_r) + one)/2.0L;
  x = (p_t == b_t
           ? one / 2.0L
           : (b_t == n_t
                  ? one * 2.0L / square(one + p_r / b_r)
                  : (n_t == p_t ? one - one * 2.0L / square(b_r / n_r + one)
                                : one / (one + p_r / b_r) +
                                      n_r * (one - p_r / b_r) /
                                          (b_r * square(one + p_r / b_r)))));

  // y = 2.0L * sqrt(n_r * b_r * p_r * (n_r + b_r + p_r))/square(b_r + n_r);
  auto t = n_r / (b_r + p_r);
  y = (b_t == p_t && n_t == p_t
           ? one * std::sqrt(3.0L) / 2.0L
           : (b_t == p_t ? sqrt(t * (one + t))
                         : 2.0L * sqrt(t * (one + t) /
                                       (one * 2.0L + p_r / b_r + b_r / p_r))));
}

IntervalPair SpiralSimilarityOperator::operator*(const IntervalPair &vec) const noexcept {
  return IntervalPair{vec.first * x - vec.second * y,
                      vec.second * x + vec.first * y};
}
SpiralSimilarityOperator SpiralSimilarityOperator::operator*(
    const SpiralSimilarityOperator &other) const noexcept {
  return SpiralSimilarityOperator{other.x * x - other.y * y,
                                  other.y * x + other.x * y};
}

SpiralSimilarityOperator &SpiralSimilarityOperator::operator=(const SpiralSimilarityOperator &other) {
  x = other.x;
  y = other.y;
  return *this;
}
SpiralSimilarityOperator &SpiralSimilarityOperator::operator=(SpiralSimilarityOperator &&other) {
  x = std::move(other.x);
  y = std::move(other.y);
  return *this;
}
SpiralSimilarityOperator::SpiralSimilarityOperator()
    : SpiralSimilarityOperator{1, 0} {}

Disk::Disk(Interval center_x_, Interval center_y_, Interval radius_,
           size_t disk_type_)
    : center_x{center_x_}, center_y{center_y_}, radius{radius_},
      disk_type{disk_type_} {};
Disk::Disk(const Disk &other) noexcept
    : center_x{other.center_x}, center_y{other.center_y}, radius{other.radius},
      disk_type{other.disk_type} {};

Disk::Disk(Disk &&other) noexcept
    : center_x{std::move(other.center_x)}, center_y{std::move(other.center_y)},
      radius{std::move(other.radius)}, disk_type{std::move(other.disk_type)} {};

Disk &Disk::operator=(const Disk &other) {
  center_x = other.center_x;
  center_y = other.center_y;
  radius = other.radius;
  disk_type = other.disk_type;
  return *this;
}
Disk &Disk::operator=(Disk &&other) {
  center_x = std::move(other.center_x);
  center_y = std::move(other.center_y);
  radius = std::move(other.radius);
  disk_type = other.disk_type;
  return *this;
}

Disk::Disk() : center_x(0, 0), center_y(0, 0), radius(0, 0), disk_type(0) {};

Interval Disk::get_norm() const { return square(center_x) + square(center_y); }
const Interval &Disk::get_radius() const { return radius; }
const Interval &Disk::get_center_x() const { return center_x; }
const Interval &Disk::get_center_y() const { return center_y; }
size_t Disk::get_type() const { return disk_type; }
BaseType Disk::precision() const {
  return std::min(width(center_x), width(center_y));
}

inline Interval gap_between_disks(const Disk &a, const Disk &b) {
  return sqrt(square(a.get_center_x() - b.get_center_x()) +
              square(a.get_center_y() - b.get_center_y())) -
         a.get_radius() - b.get_radius();
}

bool Disk::intersects(const Disk &other) const {
  return cerlt(gap_between_disks(*this, other), 0.0L);
}
bool Disk::tangent(const Disk &other) const {
  return zero_in(gap_between_disks(*this, other));
}
bool Disk::disjoint(const Disk &other) const {
  return cergt(gap_between_disks(*this, other), 0.0L);
}

bool LessNormCompare(const DiskPointer a, const DiskPointer b) {
  return median(sqrt(a->get_norm()) + a->get_radius()) <
         median(sqrt(b->get_norm()) + b->get_radius());
}

DiskClockwiseCompare::DiskClockwiseCompare(const Disk &base) {
  center_x = base.get_center_x();
  center_y = base.get_center_y();
}
bool DiskClockwiseCompare::operator()(const DiskPointer a, const DiskPointer b) const {
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