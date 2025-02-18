#include "tools.h"
#include "cw.pb.h"
#include "geometry.h"
#include "../consts.h"
#include <boost/numeric/interval/utility_fwd.hpp>

std::random_device rd;
std::mt19937 g(rd());

void DumpPacking(CDP::Packing &storage_packing, const std::list<Disk> &packing, BaseType packing_radius) {
    CDP::Disk* disk = nullptr;
    storage_packing.set_dimensions(packing_radius);
    for(auto &d : packing) {
        disk = storage_packing.add_disks();
        disk->set_radius(median(d.get_radius()));
        disk->set_x(median(d.get_center_x()));
        disk->set_y(median(d.get_center_y()));
        disk->set_size_type(d.get_type());
    }
}

void WritePackingToFile(CDP::Packing &packing, const std::string &filename){
        std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!packing.SerializeToOstream(&output)) {
            std::cerr << "Failed to write packing." << "\n";
        }
}

inline bool CheckValidity(const std::list<Disk> &packing, const Disk &new_disk) {
    for(auto &disk : packing) {
        if(new_disk.intersects(disk)) {
            return false;
        }
    }
    return true;
}

inline std::pair<Interval, Interval> GetNextCenter(const Interval &x_prime, const Interval &y_prime, const Interval &base_radius, const Interval &previous_radius,  const Interval &next_radius) {
    auto a2 = square(base_radius + previous_radius),
         b2 = square(base_radius + next_radius),
         c2 = square(previous_radius + next_radius);
    
    auto p = ((b2 - c2)/a2 + one)/2.0L,
         q = sqrt(b2/a2 - square(p));
        //  std::cout << std::setprecision(15) << p << " " << q << "\n";

        // x_1 = p * x_prime + q * y_prime,
        auto x_2 = p * x_prime - q * y_prime;
        // y_1 = p * y_prime - q * x_prime,
        auto y_2 = p * y_prime + q * x_prime;

        std::cout << std::setprecision(15) << q << " " << q << " p q\n";
        std::cout << std::setprecision(15) << x_2 << " " << y_2 << "\n";
        std::cout << std::setprecision(15) << x_prime << " " << y_prime << " huh\n";
    return std::make_pair(p * x_prime - q * y_prime, p * y_prime + q * x_prime);
}

PackingStatus GapFill(  const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const BaseType &packing_radius, QueueType &disk_queue, const Disk &base, 
                        std::vector<Disk*> &corona, const std::pair<Interval, Interval> &previous_leaf) {
                        // std::vector<Disk*> &corona, const Angle &leaf_front, const Angle &leaf_back) {
    
    // std::cout << "gapfill " <<  "\n";
    // std::cout << base.get_center_x() << " " << base.get_center_y() << " center\n";
    // assert(leaf_front.get_cos().upper() == leaf_front.get_cos().upper());
    
    assert(radii.size() == frequency_table.size());

    std::vector<size_t> shuffle(radii.size());
    for(size_t i = 0; i < radii.size(); ++i) {
        shuffle[i] = i;
    }
    std::shuffle(shuffle.begin(), shuffle.end(), g);
    if(corona.size() > 2) {
        // Angle back_to_front{radii[base.get_type()], radii[corona.back()->get_type()], radii[corona.front()->get_type()]};
        // Angle remainder{leaf_back - leaf_front + back_to_front};
        // std::cout << remainder << " remainder\n";
        // std::cout << remainder << " " << " remainder\n";
        // if (zero_in(remainder.get_sin()) && in(1.0L, remainder.get_cos())) {
        //     return AddAnotherDisk(radii, packing, frequency_table, packing_radius, disk_queue);
        // }
        // if (cergt(remainder.get_sin(), 0.0L) && cerlt((leaf_front - leaf_back).get_sin(), 0.0L)) {
        //     // std::cout << "fuck\n";
        //     // std::cout << "invalid intersects front\n";
        //     return PackingStatus::invalid;
        // }
        if(corona.back()->intersects(*corona.front())) {
            return PackingStatus::invalid;
        }
        if(corona.back()->tangent(*corona.front())) {
            // std::cout << "huh\n";
            return AddAnotherDisk(radii, packing, frequency_table, packing_radius, disk_queue);
        }
    }
    for(size_t i = 0; i < radii.size(); ++i) {

        // Angle delta_new{0};
        // if(!corona.empty()) {
        //     delta_new = Angle{base.get_radius(), radii[shuffle[i]], corona.back()->get_radius()};
        // }
        // if(corona.size() > 1) {
        //     Angle delta_alternative{base.get_radius(), radii[shuffle[i]], corona[corona.size() - 2]->get_radius()};
        //     Angle delta_old{base.get_radius(), corona.back()->get_radius(), corona[corona.size() - 2]->get_radius()};
        //     if(delta_alternative > delta_new + delta_old) {
        //         // std::cout << "disk too small fault\n";
        //         continue;
        //     }
        // }
        
        // auto new_leaf_back{leaf_back + delta_new};

        auto center = corona.empty() ? std::make_pair(base.get_radius() + radii[shuffle[i]], 0.0L) : 
        GetNextCenter(previous_leaf.first, previous_leaf.second, base.get_radius(), corona.back()->get_radius(), radii[shuffle[i]]); 

        // std::cout << center.first << " " << center.second << " center\n";

        Disk new_disk(center.first + base.get_center_x(), center.second + base.get_center_y(), radii[shuffle[i]], shuffle[i]);
        // Disk new_disk{  base.get_center_x() + new_leaf_back.get_cos() * (radii[shuffle[i]] + base.get_radius()), 
        //                 base.get_center_y() + new_leaf_back.get_sin() * (radii[shuffle[i]] + base.get_radius()), 
        //                 radii[shuffle[i]], shuffle[i]};
        // std::cout << new_disk.get_center_x() << " " << new_disk.get_center_y() << " " << new_disk.get_radius() << " new disk\n";
        if(!CheckValidity(packing, new_disk)) {
            // std::cout << "check validity fault\n";
            continue;
        }
        packing.push_back(std::move(new_disk));
        corona.push_back(&packing.back());
        disk_queue.insert(&packing.back());
        ++frequency_table[shuffle[i]];

        // packing.emplace_back(base.get_center_x(), base.get_center_y(), Interval{0.2}, 4);

        auto status = GapFill(radii, packing, frequency_table, packing_radius, disk_queue, base, corona, center);
        if(status == PackingStatus::complete) {
            // std::cout << "complete\n";
            return PackingStatus::complete;
        }
        // packing.pop_back();
        
        assert(corona.back() == &packing.back());
        corona.pop_back();
        disk_queue.erase(&packing.back());
        --frequency_table[shuffle[i]];
        packing.pop_back();
        
    }
    // std::cout << "invalid end of list\n";
    return PackingStatus::invalid;
}

inline bool IsInBounds(const Disk* disk, const BaseType &packing_radius) {
    if(disk == nullptr) {
        return false;
    }
    return cerle(disk->get_norm(), packing_radius * packing_radius);
    // return std::max(std::abs(d->get_center_x()), std::abs(d->get_center_y())) < packing_radius;
}

struct DiskClockwiseCompare{
    Interval center_x;
    Interval center_y;
    DiskClockwiseCompare(const Disk &base) {
        center_x = base.get_center_x();
        center_y = base.get_center_y();
    }
    inline bool operator()(const Disk* a, const Disk* b) const {
        auto ax = a->get_center_x() - center_x;
        auto ay = a->get_center_y() - center_y;
        auto bx = b->get_center_x() - center_x;
        auto by = b->get_center_y() - center_y;
        return  (cergt(ax, 0.0L) && zero_in(ay) ? true : 
                    (cergt(bx, 0.0L) && zero_in(by) ? false :
                        (cerlt(ay * by, 0.0L) ? cergt(ay, 0.0L) : cergt(by * ax - ay * bx, 0.0L))
                    )
                );
    }
};

inline void GetSortedCorona(const Disk &base, std::list<Disk> &packing, std::vector<Disk*> &corona) {
    assert(corona.empty());
    for(auto &disk : packing) {
        if(base.tangent(disk)) {
            corona.push_back(&disk);
        }
    }
    std::sort(corona.begin(), corona.end(), DiskClockwiseCompare{base});
    for(size_t i = 0; i + 1 < corona.size(); ++i) {
        if(!corona[i]->tangent(*corona[i + 1])) {
            std::vector<Disk*> real_corona;
            for(size_t j = 0; j < corona.size(); ++j) {
                real_corona.push_back(corona[(i + j + 1)%corona.size()]);
            }
            corona = std::move(real_corona);
            return;
        }
    }
}

inline PackingStatus GapFill(  const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const BaseType &packing_radius, QueueType &disk_queue, const Disk &base) {
    std::vector<Disk*> corona(0);
    GetSortedCorona(base, packing, corona);
    // for(size_t i = 0; i < corona.size(); ++i) {
    //     // std::cout << corona[i]->get_center_x() - base.get_center_x() << " " << corona[i]->get_center_y() - base.get_center_y() << "\n";
    // }
    for(size_t i = 0; i + 1< corona.size(); ++i) {
        if(!corona[i]->tangent(*corona[i + 1])) {
            std::cout << "fuck\n";
            packing.emplace_back(base.get_center_x(), base.get_center_y(), Interval{0.2}, 4);
            return PackingStatus::complete;
        }
        // assert(corona[i]->tangent(*corona[i + 1]));
    }

    // Angle leaf_back{starting_angle}, leaf_front{starting_angle};
    // if(!corona.empty()) {
    //     leaf_back = Angle{  (corona.back()->get_center_x() - base.get_center_x())/(corona.back()->get_radius() + base.get_radius()), 
    //                         (corona.back()->get_center_y() - base.get_center_y())/(corona.back()->get_radius() + base.get_radius())
    //                     };
    //     leaf_front = Angle{ (corona.front()->get_center_x() - base.get_center_x())/(corona.front()->get_radius() + base.get_radius()), 
    //                         (corona.front()->get_center_y() - base.get_center_y())/(corona.front()->get_radius() + base.get_radius())
    //                     };
    // }
    
    return GapFill(radii, packing, frequency_table, packing_radius, disk_queue, base, corona, 
    (corona.empty() ? std::make_pair(0.0L, 0.0L) : std::make_pair(corona.back()->get_center_x() - median(base.get_center_x()),corona.back()->get_center_y() - median(base.get_center_y()))));
}

PackingStatus AddAnotherDisk(const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const BaseType &packing_radius, QueueType &disk_queue) {
    // std::cout << "add\n";
    Disk* base = nullptr;
    while(!IsInBounds(base, packing_radius) && !disk_queue.empty()) {
        // for(auto &x : disk_queue) {
        //     std::cout << x->get_center_x() << " " << x->get_center_y() << ' ' << x->get_radius() << " " << x->get_norm() << " BLEATI\n";
        // } 
        base = *(disk_queue.begin());
        disk_queue.erase(disk_queue.begin());
    }
    if(!IsInBounds(base, packing_radius)) {
        return (std::find(frequency_table.begin(), frequency_table.end(), 0) != frequency_table.end() ? PackingStatus::invalid : PackingStatus::complete); 
    }
    // std::cout << median(sqrt(base->get_norm()) + base->get_radius()) << " hell yeah\n";
    auto status = GapFill(radii, packing, frequency_table, packing_radius, disk_queue, *base);
    if(status == PackingStatus::invalid) {
        disk_queue.insert(base);
    }

    return status;
}
PackingStatus FindPacking(const std::vector<Interval> &radii, const BaseType &packing_radius, const std::string &storage_file) {
    QueueType disk_queue(LessNormCompare);
    std::list<Disk> packing;
    std::vector<int> frequency_table(radii.size(), 0);
    packing.emplace_back(zero, zero, radii[0], 0);
    disk_queue.insert(&packing.back());
    ++frequency_table[0];
    auto status = AddAnotherDisk(radii, packing, frequency_table, packing_radius, disk_queue);
    if(status == PackingStatus::complete) {
        CDP::Packing storage_packing;
        DumpPacking(storage_packing, packing, packing_radius);
        WritePackingToFile(storage_packing, storage_file);        
    }
    return status;
}