#include <diskpack/geometry.h>
#include <list>

#pragma once

namespace diskpack {
    void WritePackingSVG(const std::string &filename, const std::list<DiskPointer> &packing, BaseType packing_radius);
    std::string EncodeRegionsJSON(const std::vector<RadiiRegion>& regions);
    void DecodeRegionsJSON(std::istream& data, std::vector<RadiiRegion> &regions);
} // namespace diskpack