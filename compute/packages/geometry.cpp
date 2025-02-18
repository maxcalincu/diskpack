#include "geometry.h"
#include "../consts.h"

std::ostream& operator<<(std::ostream& out, const Interval& x) {
    return out << x.lower() << " " << x.upper();
}

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
    return  cerlt(gap_between_disks(*this, other), 0.0L);
}
bool Disk::tangent(const Disk &other) const {
    return  zero_in(gap_between_disks(*this, other));
}
bool Disk::disjoint(const Disk &other) const {
    return  cergt(gap_between_disks(*this, other), 0.0L);
}

bool LessNormCompare(const Disk *a, const Disk *b) {
    return median(sqrt(a->get_norm()) + a->get_radius()) < median(sqrt(b->get_norm()) + b->get_radius());
}

/*

x^2 + y^2 = (r1 + r)^2
x2^2 + y2^2 = (r1 + r2)^2
(x2 - x)^2 + (y2 - y)^2 = (r2 + r)^2


x2x + y2y = r1 ^ 2 + r1r + r1r2 - r2r = a

*/