#include <bits/stdc++.h>
#include <boost/numeric/interval.hpp>
#pragma once

using namespace boost::numeric;

using BaseType = long double;                                                     /// using alias in case one wants to swap long double with a more precise type

using Interval = interval<                                                        /// Interval arithmetic is used
    BaseType,
    interval_lib::policies<
        interval_lib::save_state<interval_lib::rounded_transc_exact<BaseType>>,
        interval_lib::checking_base<BaseType>>>;
                                                                             
using IntervalPair = std::pair<Interval, Interval>;                               

const BaseType epsilon = .00000000001;

const Interval one{1 - epsilon, 1 + epsilon}, zero{-epsilon, epsilon};

/// Spiral similarity operator is used to compute the position of a disk tangent to two other disks 
struct SpiralSimilarityOperator {                                                   
  Interval x;
  Interval y;

  SpiralSimilarityOperator(const Interval &x_, const Interval &y_);
  SpiralSimilarityOperator(const SpiralSimilarityOperator &other);
  SpiralSimilarityOperator(SpiralSimilarityOperator &&other);

  SpiralSimilarityOperator &operator=(const SpiralSimilarityOperator &other);
  SpiralSimilarityOperator &operator=(SpiralSimilarityOperator &&other);

  SpiralSimilarityOperator(const Interval &base_r, const Interval &prev_r,
                           const Interval &next_r, const size_t &base_t,
                           const size_t &prev_t, const size_t &next_t);

  IntervalPair operator*(const IntervalPair &vec) const noexcept;
  SpiralSimilarityOperator
  operator*(const SpiralSimilarityOperator &other) const noexcept;
  SpiralSimilarityOperator();
};

std::ostream &operator<<(std::ostream &out, const Interval &x);

#define INSPECT(expr) std::cout << #expr << ": " << (expr) << "\n";

/// Disk class
class Disk {                                                                        
  Interval center_x, center_y, radius;
  size_t disk_type;

public:
  Disk(Interval center_x_, Interval center_y_, Interval radius_,
       size_t disk_type_);
  Disk();
  Disk(const Disk &other) noexcept;
  Disk(Disk &&other) noexcept;
  Disk &operator=(const Disk &other);
  Disk &operator=(Disk &&other);

  Interval get_norm() const;
  Interval get_radius() const;
  Interval get_center_x() const;
  Interval get_center_y() const;
  size_t get_type() const;
  bool intersects(const Disk &other) const;
  bool tangent(const Disk &other) const;
  bool disjoint(const Disk &other) const;
  BaseType precision() const;
};
bool LessNormCompare(const Disk *a, const Disk *b);