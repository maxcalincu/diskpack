#include "geometry.h"
#pragma once

enum PackingStatus {complete, invalid};

typedef std::multiset<Disk*, decltype(&LessNormCompare)> QueueType;
typedef std::vector<const SpiralSimilarityOperator*> OperatorCollection;

class OperatorLookUpTable {
    std::vector<SpiralSimilarityOperator> values;
    std::vector<bool> presence;
    SpiralSimilarityOperator identity;
    const std::vector<Interval> &radii;
    inline size_t GetIndex(size_t i, size_t j, size_t k);
public:
    SpiralSimilarityOperator* operator()(size_t base_type, size_t prev_type, size_t next_type);
    SpiralSimilarityOperator* operator()();
    OperatorLookUpTable(const std::vector<Interval> &radii_);
};


class PackingGenerator {

    const std::vector<Interval> radii;
    const BaseType packing_radius;
    std::list<Disk> packing;
    QueueType disk_queue;
    OperatorLookUpTable lut;

    std::vector<int> frequency_table;

    PackingStatus GapFill(const Disk& base, std::vector<Disk*> &corona, std::vector<SpiralSimilarityOperator*> &operators, IntervalPair &starting_leaf);
    PackingStatus AddAnotherDisk();
    inline void GetSortedCorona(const Disk &base, std::vector<Disk*> &corona);


public:
    PackingGenerator(const std::vector<Interval> &radii_, const BaseType &packing_radius_);
    PackingStatus FindPacking(const std::string &storage_file);
    void Reset();
    inline bool IsInBounds(const Disk* disk);
    inline bool HasIntersection(const Disk &new_disk);
};