#include "geometry.h"
// #include <algorithm>
// #include <iomanip>
#include "../consts.h"

// const Interval trigonometry_interval = ;

std::ostream& operator<<(std::ostream& out, const Interval& x) {
    return out << x.lower() << " " << x.upper();
}

// Interval cosine(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius) {
//     auto result = 1.0L - (2.0L * a_radius * b_radius) / ((base_radius + a_radius) * (base_radius + b_radius)); 
//     return intersect(result, Interval{-1, 1});
// }
// Interval sine(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius) {
//     auto result = sqrt(4.0L * a_radius * b_radius * (square(base_radius) + base_radius * (a_radius + b_radius))) / ((base_radius + a_radius) * (base_radius + b_radius));
//     return intersect(result, Interval{-1, 1});
// }
    
// Angle::Angle(): Angle(0) {};
// Angle::Angle(const BaseType &phi): cos{std::cos(phi)}, sin{std::sin(phi)}{}
// Angle::Angle(const Interval &base_radius, const Interval &a_radius, const Interval &b_radius): cos{cosine(base_radius, a_radius, b_radius)}, sin{sine(base_radius, a_radius, b_radius)} {
//     cos = intersect(cos, Interval{-1, 1});
//     sin = intersect(sin, Interval{-1, 1});
// }
// Angle::Angle(const Interval &cos_, const Interval &sin_): cos{cos_}, sin{sin_} {
//     cos = intersect(cos, Interval{-1, 1});
//     sin = intersect(sin, Interval{-1, 1});
// }
// Angle::Angle(const Angle &other): cos{other.cos},  sin{other.sin} {}
// Angle::Angle(Angle &&other): cos{std::move(other.cos)},  sin{std::move(other.sin)} {}

// Angle& Angle::operator=(const Angle &other) {
//     cos = other.cos;
//     sin = other.sin;
//     return *this;
// }

// Angle& Angle::operator=(Angle &&other) {
//     cos = std::move(other.cos);
//     sin = std::move(other.sin);
//     return *this;
// }

// Interval Angle::get_sin() const {
//     return sin;
// }
// Interval Angle::get_cos() const {
//     return cos;
// }

// void GetEndpoints(const Angle &x, BaseType &a_x, BaseType &a_y, BaseType &b_x, BaseType &b_y) {
//     auto cos_ = x.get_cos(), sin_ = x.get_sin();
//     a_x = (sin_.lower() > 0 ? cos_.lower() : cos_.upper());
//     b_x = (sin_.upper() < 0 ? cos_.lower() : cos_.upper());

//     a_y = (cos_.upper() < 0 ? sin_.lower() : sin_.upper());
//     b_y = (cos_.lower() > 0 ? sin_.lower() : sin_.upper());

    // if(cos_.upper() >= 1.0L) {
    //     a_x = cos_.lower();
    //     b_x = cos_.lower();
    // }
    // if(cos_.lower() <= -1.0L) {
    //     a_x = cos_.upper();
    //     b_x = cos_.upper();;
    // }

    // if(sin_.upper() >= 1.0L) {
    //     a_y = sin_.lower();
    //     b_y = sin_.lower();
    // }
    // if(sin_.lower() <= -1.0L) {
    //     a_y = sin_.upper();
    //     b_y = sin_.upper();
    // }
// }


// Angle Angle::operator+(const Angle &other) const {
//     // return Angle{   intersect(other.get_cos() * this->get_cos() - other.get_sin() * this->get_sin(), Interval{-1, 1}), 
//                     // intersect(other.get_cos() * this->get_sin() + other.get_sin() * this->get_cos(), Interval{-1, 1})};
//     BaseType a_x, a_y, b_x, b_y, c_x, c_y, d_x, d_y;

//     GetEndpoints(*this, a_x, a_y, b_x, b_y);
//     GetEndpoints(other, c_x, c_y, d_x, d_y);

//     std::vector<BaseType> cos_candidates = {
//                                             cos.upper() * other.cos.upper() - sin.upper() * other.sin.upper(),
//                                             cos.lower() * other.cos.lower() - sin.lower() * other.sin.lower(),
//                                             // a_x * c_x - a_y * c_y,
//                                             // a_x * d_x - a_y * d_y,
//                                             // b_x * c_x - b_y * c_y,
//                                             // b_x * d_x - b_y * d_y,
//                                             };
//     std::vector<BaseType> sin_candidates = {
//                                             sin.lower() * other.cos.lower() - cos.lower() * other.sin.lower(),
//                                             sin.upper() * other.cos.upper() - cos.upper() * other.sin.upper(),
//                                             // a_y * c_x + a_x * c_y,
//                                             // a_y * d_x + a_x * d_y,
//                                             // b_y * c_x + b_x * c_y,
//                                             // b_y * d_x + b_x * d_y,
//                                             };

//     BaseType sin_1 = *std::min_element(sin_candidates.begin(), sin_candidates.end());
//     BaseType sin_2 = *std::max_element(sin_candidates.begin(), sin_candidates.end());
//     BaseType cos_1 = *std::min_element(cos_candidates.begin(), cos_candidates.end());
//     BaseType cos_2 = *std::max_element(cos_candidates.begin(), cos_candidates.end());

//     if(sin_1 * sin_2 < 0) {
//         if (cos_2 < 0) cos_1 = -1.0L;
//         if (cos_1 > 0) cos_2 = 1.0L;
//     }
//     if(cos_1 * cos_2 < 0) {
//         if (sin_2 < 0) sin_1 = -1.0L;
//         if (sin_1 > 0) sin_2 = 1.0L;
//     }
//     if(sin_1 > sin_2) std::swap(sin_1, sin_2);
//     if(cos_1 > cos_2) std::swap(cos_1, cos_2);
    
//     return Angle{intersect(Interval{cos_1, cos_2}, Interval{-1, 1}), intersect(Interval{sin_1, sin_2}, Interval{-1, 1})};
// }

// Angle Angle::operator-(const Angle &other) const {
//     // return Angle{   intersect(other.get_cos() * this->get_cos() + other.get_sin() * this->get_sin(), Interval{-1, 1}), 
//                     // intersect(other.get_cos() * this->get_sin() - other.get_sin() * this->get_cos(), Interval{-1, 1})};
//     BaseType a_x, a_y, b_x, b_y, c_x, c_y, d_x, d_y;

//     GetEndpoints(*this, a_x, a_y, b_x, b_y);
//     GetEndpoints(other, c_x, c_y, d_x, d_y);

//     std::vector<BaseType> cos_candidates = {
//                                             cos.upper() * other.cos.lower() + sin.upper() * other.sin.lower(),
//                                             cos.lower() * other.cos.upper() + sin.lower() * other.sin.upper(),
//                                             // a_x * c_x + a_y * c_y,
//                                             // a_x * d_x + a_y * d_y,
//                                             // b_x * c_x + b_y * c_y,
//                                             // b_x * d_x + b_y * d_y,
//                                             };
//     std::vector<BaseType> sin_candidates = {
//                                             sin.upper() * other.cos.lower() - cos.upper() * other.sin.lower(),
//                                             sin.lower() * other.cos.upper() - cos.lower() * other.sin.upper(),
//                                             // a_y * c_x - a_x * c_y,
//                                             // a_y * d_x - a_x * d_y,
//                                             // b_y * c_x - b_x * c_y,
//                                             // b_y * d_x - b_x * d_y,
//                                             };

//     BaseType sin_1 = *std::min_element(sin_candidates.begin(), sin_candidates.end());
//     BaseType sin_2 = *std::max_element(sin_candidates.begin(), sin_candidates.end());
//     BaseType cos_1 = *std::min_element(cos_candidates.begin(), cos_candidates.end());
//     BaseType cos_2 = *std::max_element(cos_candidates.begin(), cos_candidates.end());

//     if(sin_1 * sin_2 < 0) {
//         if (cos_2 < 0) cos_1 = -1.0L;
//         if (cos_1 > 0) cos_2 = 1.0L;
//     }
//     if(cos_1 * cos_2 < 0) {
//         if (sin_2 < 0) sin_1 = -1.0L;
//         if (sin_1 > 0) sin_2 = 1.0L;
//     }
//     if(sin_1 > sin_2) std::swap(sin_1, sin_2);
//     if(cos_1 > cos_2) std::swap(cos_1, cos_2);

//     return Angle{intersect(Interval{cos_1, cos_2}, Interval{-1, 1}), intersect(Interval{sin_1, sin_2}, Interval{-1, 1})};
// }
// Angle& Angle::operator+=(const Angle &other) {
//     *this = *this + other;
//     return *this;
// }
// Angle& Angle::operator-=(const Angle &other) {
//     *this = *this - other;
//     return *this;
// }

// std::ostream& operator<<(std::ostream& out, const Angle& a) {
//     return out << std::setprecision(10) << "sin: " << a.get_sin() << " cos: " << a.get_cos();
// }

// bool Angle::operator<(const Angle &other) const {
//     //angles \in \[0, \pi\]

//     return cos.lower() > other.cos.upper();
// }

// bool Angle::operator>(const Angle &other) const {
//     //angles \in [0, \pi]

//     return other.cos.lower() > cos.upper();
// }

Disk::Disk(Interval center_x_, Interval center_y_, Interval radius_, size_t disk_type_):    center_x{center_x_}, 
                                                                                            center_y{center_y_}, 
                                                                                            radius{radius_}, 
                                                                                            disk_type{disk_type_} {};
Disk::Disk(const Disk &other) noexcept:     center_x{other.center_x}, 
                                            center_y{other.center_y}, 
                                            radius{other.radius}, 
                                            disk_type{other.disk_type} {};

Disk::Disk(const Disk &&other) noexcept:    center_x{std::move(other.center_x)}, 
                                            center_y{std::move(other.center_y)}, 
                                            radius{std::move(other.radius)}, 
                                            disk_type{std::move(other.disk_type)} {};

Interval Disk::get_norm() const {return square(center_x) + square(center_y);}
Interval Disk::get_radius() const {return radius;}
Interval Disk::get_center_x() const {return center_x;}
Interval Disk::get_center_y() const {return center_y;}
size_t Disk::get_type() const {return disk_type;}

inline Interval gap_between_disks(const Disk &a, const Disk &b) {
    return sqrt(square(a.get_center_x() - b.get_center_x()) + square(a.get_center_y() - b.get_center_y())) - a.get_radius() - b.get_radius();
}

bool Disk::intersects(const Disk &other) const {
    // std::cout << gap_between_disks(*this, other) << " gap\n";
    // std::cout << this->get_center_x() << " " << this->get_center_y() << " this\n";
    // std::cout << other.get_center_x() << " " << other.get_center_y() << " other\n";
    return  cerlt(gap_between_disks(*this, other), 0.0L);
}
bool Disk::tangent(const Disk &other) const {
    return  zero_in(gap_between_disks(*this, other));
}
bool Disk::disjoint(const Disk &other) const {
    return  cergt(gap_between_disks(*this, other), 0.0L);
}

bool LessNormCompare(const Disk *a, const Disk *b) {
    // std::cout << sqrt(a->get_norm()) + a->get_radius() << " " << sqrt(b->get_norm()) + b->get_radius() << " LessNormCompare\n";
    // return cerlt(sqrt(a->get_norm()) + a->get_radius(), sqrt(b->get_norm()) + b->get_radius());
    return median(sqrt(a->get_norm()) + a->get_radius()) < median(sqrt(b->get_norm()) + b->get_radius());
}

/*

x^2 + y^2 = (r1 + r)^2
x2^2 + y2^2 = (r1 + r2)^2
(x2 - x)^2 + (y2 - y)^2 = (r2 + r)^2


x2x + y2y = r1 ^ 2 + r1r + r1r2 - r2r = a

*/