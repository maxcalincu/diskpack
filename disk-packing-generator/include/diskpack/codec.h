#include <diskpack/geometry.h>
#include <list>

namespace CDP {
    /// Helper functions for storing packings
    void WritePackingSVG(const std::string &filename, const std::list<Disk> &packing, BaseType packing_radius);
    void WritePackingPROTO(const std::string &filename, const std::list<Disk> &packing, BaseType packing_radius);
}