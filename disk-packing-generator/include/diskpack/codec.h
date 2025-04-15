#include <diskpack/geometry.h>
#include <list>

namespace diskpack {
    /// Helper functions for storing packings
    void WritePackingSVG(const std::string &filename, const std::list<DiskPointer> &packing, BaseType packing_radius);
    void WritePackingPROTO(const std::string &filename, const std::list<DiskPointer> &packing, BaseType packing_radius);
}