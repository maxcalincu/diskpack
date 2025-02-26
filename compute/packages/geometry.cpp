#include "geometry.h"

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

  x = (p_t == b_t
           ? one / 2.0L
           : (b_t == n_t
                  ? one * 2.0L / square(one + p_r / b_r)
                  : (n_t == p_t ? one - one * 2.0L / square(b_r / n_r + one)
                                : one / (one + p_r / b_r) +
                                      n_r * (one - p_r / b_r) /
                                          (b_r * square(one + p_r / b_r)))));

  // x = ((square(n_r + p_r) - square(n_r + p_r))/square(n_r + b_r) + one)/2.0L;

  //   (p^2 + np - nb + pb) / (p + b)^2
  //   p_r/(p_r + b_r) + n_r(p_r - b_r)/(p_r + b_r)^2

  // y = 2.0L * sqrt(n_r * b_r * p_r * (n_r + b_r + p_r))/square(b_r + n_r);
  auto t = n_r / (b_r + p_r);
  y = (b_t == p_t && n_t == p_t
           ? one * std::sqrt(3.0L) / 2.0L
           : (b_t == p_t ? sqrt(t * (one + t))
                         : 2.0L * sqrt(t * (one + t) /
                                       (one * 2.0L + p_r / b_r + b_r / p_r))));
}

IntervalPair
SpiralSimilarityOperator::operator*(const IntervalPair &vec) const noexcept {
  return IntervalPair{vec.first * x - vec.second * y,
                      vec.second * x + vec.first * y};
}
SpiralSimilarityOperator SpiralSimilarityOperator::operator*(
    const SpiralSimilarityOperator &other) const noexcept {
  return SpiralSimilarityOperator{other.x * x - other.y * y,
                                  other.y * x + other.x * y};
}

SpiralSimilarityOperator &
SpiralSimilarityOperator::operator=(const SpiralSimilarityOperator &other) {
  x = other.x;
  y = other.y;
  return *this;
}
SpiralSimilarityOperator &
SpiralSimilarityOperator::operator=(SpiralSimilarityOperator &&other) {
  x = std::move(other.x);
  y = std::move(other.y);
  return *this;
}
SpiralSimilarityOperator::SpiralSimilarityOperator()
    : SpiralSimilarityOperator{1, 0} {}

std::ostream &operator<<(std::ostream &out, const Interval &x) {
  return out << x.lower() << " " << x.upper();
}

Disk::Disk(Interval center_x_, Interval center_y_, Interval radius_,
           size_t disk_type_)
    : center_x{center_x_}, center_y{center_y_}, radius{radius_},
      disk_type{disk_type_} {};
Disk::Disk(const Disk &other) noexcept
    : center_x{other.center_x}, center_y{other.center_y}, radius{other.radius},
      disk_type{other.disk_type} {};

Disk::Disk(const Disk &&other) noexcept
    : center_x{std::move(other.center_x)}, center_y{std::move(other.center_y)},
      radius{std::move(other.radius)}, disk_type{std::move(other.disk_type)} {};

Interval Disk::get_norm() const { return square(center_x) + square(center_y); }
Interval Disk::get_radius() const { return radius; }
Interval Disk::get_center_x() const { return center_x; }
Interval Disk::get_center_y() const { return center_y; }
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

bool LessNormCompare(const Disk *a, const Disk *b) {
  return median(sqrt(a->get_norm()) + a->get_radius()) <
         median(sqrt(b->get_norm()) + b->get_radius());
}