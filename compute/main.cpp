#include <bits/stdc++.h>
#include "packages/geometry.h"
#include "consts.h"
#include "packages/tools.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
std::string storage_file = "../storage/1.txt";

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:  " << argv[0] << " storage_file" << std::endl;
        return -1;
    }
    
    // std::cout << intersect(starting_angle.get_cos(), trigonometry_interval) << "\n";
    // for(size_t i = 1; i<10; ++i) {
        int i;
        BaseType packing_radius;
        std::sscanf(argv[1], "%d", &i);
        std::sscanf(argv[2], "%Lf", &packing_radius);
        std::vector<size_t> coronal_code(0);
        std::vector<Interval> radii{one, d[i].first, d[i].second};//, d[i].first, d[i].second};

        storage_file = std::string{"../storage/"} + char('0') + std::string{".txt"};

        auto t1 = high_resolution_clock::now();
        auto status = FindPacking(radii, packing_radius, storage_file);
        auto t2 = high_resolution_clock::now();
        
        auto ms_int = duration_cast<milliseconds>(t2 - t1);
        std::cout << ms_int.count() << "ms\n";
        if (status == PackingStatus::complete) std::cout << "complete\n"; else std::cout << "invalid\n";
        
    // }

    return 0;
}


// c1 0.6375559772 1111r
// c2 0.5451510421 111rr
// c3 0.5332964167 1r1r1
// c4 0.4142135624 1111
// c5 0.3861061049 1rrr1
// c6 0.3491981862 1rr1r
// c7 0.2807764064 111r
// c8 0.1547005384 111
// c9 0.1010205144 11rr
