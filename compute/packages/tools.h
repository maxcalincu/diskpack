#include "geometry.h"
#pragma once

enum PackingStatus {complete, invalid};

typedef std::multiset<Disk*, decltype(&LessNormCompare)> QueueType;

class PackingGenerator {
    std::vector<Interval> radii;
    BaseType packing_radius;
    std::list<Disk> packing;
    QueueType disk_queue;
    std::vector<int> frequency_table;

    PackingStatus GapFill(const Disk& base, std::vector<Disk*> &corona, const std::pair<Interval, Interval> &previous_leaf);
    PackingStatus AddAnotherDisk();
    inline void GetSortedCorona(const Disk &base, std::vector<Disk*> &corona);


public:
    PackingGenerator(const std::vector<Interval> &radii_, const BaseType &packing_radius_);
    PackingStatus FindPacking(const std::string &storage_file);
    void Reset();
    inline bool IsInBounds(const Disk* disk);
    inline bool HasIntersection(const Disk &new_disk);
};