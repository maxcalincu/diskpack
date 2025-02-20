#include <bits/stdc++.h>
#include <boost/numeric/interval.hpp>
#pragma once

using namespace boost::numeric;

typedef long double BaseType;

typedef interval<BaseType, interval_lib::policies<interval_lib::save_state<interval_lib::rounded_transc_exact<BaseType> >, interval_lib::checking_base<BaseType> > > Interval;
typedef std::pair<Interval, Interval> IntervalPair;

const BaseType epsilon = .00000000001;

const Interval  one  {1 - epsilon, 1 + epsilon}, 
                zero{-epsilon, epsilon};

struct SpiralSimilarityOperator {
    Interval x;
    Interval y;

    SpiralSimilarityOperator(const Interval &x_, const Interval &y_);
    SpiralSimilarityOperator(const SpiralSimilarityOperator &other);
    SpiralSimilarityOperator(SpiralSimilarityOperator &&other);

    SpiralSimilarityOperator& operator=(const SpiralSimilarityOperator &other);
    SpiralSimilarityOperator& operator=(SpiralSimilarityOperator &&other);

    SpiralSimilarityOperator(const Interval &base_radius, const Interval &previous_radius,  const Interval &next_radius);

    IntervalPair operator*(const IntervalPair &vec) const noexcept;
    SpiralSimilarityOperator operator*(const SpiralSimilarityOperator &other) const noexcept;
    SpiralSimilarityOperator();
};


std::ostream& operator<<(std::ostream& out, const Interval& x);

#define INSPECT(expr) std::cout << #expr << ": " << (expr) << "\n";

class Disk {
    Interval center_x, center_y, radius;
    size_t disk_type;
public:

    Disk(Interval center_x_, Interval center_y_, Interval radius_, size_t disk_type_);
    Disk(const Disk &other) noexcept;
    Disk(const Disk &&other) noexcept;

    Interval get_norm() const;
    Interval get_radius() const;
    Interval get_center_x() const;
    Interval get_center_y() const;
    size_t get_type() const;
    bool intersects(const Disk &other) const;
    bool tangent(const Disk &other) const;
    bool disjoint(const Disk &other) const;
};
bool LessNormCompare(const Disk *a, const Disk *b);