#include <iostream>
#include <chrono>


#include "src/MwBarCode.h"

using namespace Magick;
using namespace std::chrono;


int main(int ac, const char* av[]) {

    MwBarCode app {ac, av};


    if (!app.options_ok)
    {
        return 1;
    }

    auto in_dir         = app.get_option<path>("in-dir");
    auto out_img        = app.get_option<path>("out-file");
    auto no_of_threads  = app.get_option<size_t>("threads");


    if (in_dir)
    {
        app.read_in_dir(*in_dir);
    }

    if (*no_of_threads < 1)
    {
        *no_of_threads = 1;
    }


    // get all image paths in an input folder
    vector<path> found_files = app.getPaths();

    // create image processing threads
    ProcessImages process_images {*no_of_threads, found_files};


    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    process_images.start_threads();
    process_images.join_threads();

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();

    fmt::print("Time measured: {:d}\n", duration);

    ProcessImages::out_vector out_values;
    out_values = process_images.get_results();

    Magick::Image bar_code;
    bar_code = app.makeBarCode(out_values);


    bar_code.write((*out_img).string());

   // app.test();

    cout << "test" << endl;

    return 0;
};
