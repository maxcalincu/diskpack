#include <bits/stdc++.h>
#include <boost/numeric/interval.hpp>
#pragma once

using namespace boost::numeric;

typedef long double BaseType;

typedef interval<BaseType, interval_lib::policies<interval_lib::save_state<interval_lib::rounded_transc_exact<BaseType> >, interval_lib::checking_base<BaseType> > > Interval;

std::ostream& operator<<(std::ostream& out, const Interval& x);

#define INSPECT(expr) std::cout << #expr << ": " << (expr) << "\n";

// Interval cosine(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius);
// Interval sine(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius);

// class Angle {
//     Interval cos;
//     Interval sin;
    
//     public:
    
//     Angle();
//     Angle(const BaseType &phi);
//     explicit Angle(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius);
//     explicit Angle(const Interval &cos_, const Interval &sin_);
//     Angle(const Angle &other);
//     Angle(Angle &&other);
    
//     Angle& operator=(const Angle &other);

//     Angle& operator=(Angle &&other);
    
//     Interval get_sin() const;
//     Interval get_cos() const;
//     Angle operator+(const Angle &other) const;
//     Angle operator-(const Angle &other) const;
//     Angle& operator+=(const Angle &other);
//     Angle& operator-=(const Angle &other);
//     bool operator<(const Angle &other) const;
//     bool operator>(const Angle &other) const;

// };
// std::ostream& operator<<(std::ostream& out, const Angle& a);

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