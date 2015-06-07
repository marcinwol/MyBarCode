#include <iostream>
#include <chrono>


#include "src/MwBarCode.h"
#include "src/ProcessImages.h"

using namespace Magick;
using namespace std::chrono;


int main(int ac, const char* av[]) {

    MwBarCode app {ac, av};


    if (!app.options_ok)
    {
        return 1;
    }

    auto in_dir         = app.get_option<path>("in-dir");


    if (in_dir)
    {
        app.read_in_dir(*in_dir);
    }



    // get all image paths in an input folder
    vector<path> found_files = app.getPaths();
    ProcessImages process_images {1, found_files};


    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    process_images.start_threads();
    process_images.join_threads();

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();

    fmt::print("Time measured: {:d}\n", duration);

    ProcessImages::out_vector out_values;
    out_values = process_images.get_results();

    vector<MwColor> avg_pixels;

    transform(out_values.begin(), out_values.end(),
              back_inserter(avg_pixels),
              [](const ProcessImages::out_struct& os) {return os.mwc;});


    Magick::Image bar_code;
    bar_code = app.makeBarCode(avg_pixels);

    bar_code.write("/home/marcin/Desktop/out.png");

   // app.test();

    cout << "test" << endl;

    return 0;
};
