#include <diskpack/codec.h>
#include <fstream>
#include <vector>
#include <iostream>

const std::vector<std::string> type_to_color = {
    "#ff8080a0",
    "#aa0087a0",
    "#ff0066a0",
    "#803dc1a0",
    "#000000a0",
};

namespace diskpack {
    // void WritePackingPROTO(const std::string &filename,
    //                 const std::list<Disk> &packing, BaseType packing_radius) {
    //     ProtoPacking storage_packing;
    //     ProtoDisk *disk = nullptr;
    //     storage_packing.set_dimensions(packing_radius);
    
    //     for (auto &d : packing) {
    //         disk = storage_packing.add_disks();
    //         disk->set_radius(median(d.get_radius()));
    //         disk->set_x(median(d.get_center_x()));
    //         disk->set_y(median(d.get_center_y()));
    //         disk->set_size_type(d.get_type());
    //     }
    
    //     std::fstream output(filename,
    //                         std::ios::out | std::ios::trunc | std::ios::binary);
    //     if (!storage_packing.SerializeToOstream(&output)) {
    //         std::cerr << "Failed to write packing\n";
    //     }
    // }

    void WritePackingSVG(const std::string &filename,
                            const std::list<Disk> &packing, BaseType packing_radius) {
        std::ofstream svgFile(filename);
        int width = 1000, height = 1000;
        
        if (!svgFile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        // SVG header
        svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
        svgFile << "<svg viewBox=\"" << -packing_radius << " " << -packing_radius << " " << 2*packing_radius << " " << 2*packing_radius
                << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        
        svgFile << "  <rect x=\"" << -packing_radius << "\" y=\"" << -packing_radius << "\" "
                << "width=\"" << 2*packing_radius << "\" height=\"" << 2*packing_radius << "\" "
                << "fill=\"white\"/>\n";

        svgFile << "  <g transform=\"scale(1, -1)\">\n";
        
        for (const auto& disk : packing) {
            svgFile << "  <circle" 
                    << " cx=\"" << median(disk.get_center_x()) 
                    << "\" cy=\"" << median( disk.get_center_y()) 
                    << "\" r=\"" << median(disk.get_radius()) 
                    << "\" fill=\"" << type_to_color[disk.get_type()] << "\"/>\n";
        }
        
        svgFile << "  </g>\n";
        svgFile << "</svg>\n";
        
        svgFile.close();
        std::cerr << "SVG file created successfully: " << filename << std::endl;
    }
}