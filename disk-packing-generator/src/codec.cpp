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
    void WritePackingSVG(const std::string &filename,
                            const std::list<DiskPointer> &packing, BaseType packing_radius) {
        std::ofstream svgFile(filename);
        int width = 1000, height = 1000;
        
        if (!svgFile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
        svgFile << "<svg viewBox=\"" << -packing_radius << " " << -packing_radius << " " << 2*packing_radius << " " << 2*packing_radius
                << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        
        svgFile << "  <rect x=\"" << -packing_radius << "\" y=\"" << -packing_radius << "\" "
                << "width=\"" << 2*packing_radius << "\" height=\"" << 2*packing_radius << "\" "
                << "fill=\"white\"/>\n";

        svgFile << "  <g transform=\"scale(1, -1)\">\n";
        
        for (const auto& disk : packing) {
            svgFile << "  <circle" 
                    << " cx=\"" << median(disk->get_center_x()) 
                    << "\" cy=\"" << median( disk->get_center_y()) 
                    << "\" r=\"" << median(disk->get_radius()) 
                    << "\" fill=\"" << type_to_color[disk->get_type()] << "\"/>\n";
        }
        
        svgFile << "  </g>\n";
        svgFile << "</svg>\n";
        
        svgFile.close();
        std::cerr << "SVG file created successfully: " << filename << std::endl;
    }
}