#include "geometry.h"
#pragma once

enum PackingStatus {complete, invalid};

typedef std::multiset<Disk*, decltype(&LessNormCompare)> QueueType;


inline PackingStatus GapFill(const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const  BaseType &packing_radius, QueueType &disk_queue, const Disk &base);
PackingStatus GapFill(  const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const BaseType &packing_radius, QueueType &disk_queue, const Disk& base, 
                        std::vector<Disk*> &corona, const std::pair<Interval, Interval> &previous_leaf);
PackingStatus AddAnotherDisk(const std::vector<Interval> &radii, std::list<Disk> &packing, std::vector<int> &frequency_table, const BaseType &packing_radius, QueueType &disk_queue);
PackingStatus FindPacking(const std::vector<Interval> &radii, const BaseType &packing_radius, const std::string &storage_file);