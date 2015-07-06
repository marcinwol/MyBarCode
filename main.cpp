#include <iostream>
#include <chrono>
#include <iomanip>

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
    auto check_types    = app.get_option<bool>("check-types");
    auto verbose        = app.get_option<bool>("verbose");
    auto sort_files     = app.get_option<bool>("sort-files");

    if (*no_of_threads < 1)
    {
        *no_of_threads = 1;
    }

    MwBarCode::VERBOSE = *verbose;

    if (in_dir)
    {
        // read in all image files found in the *in_dir
        cout << "Reading files in " << *in_dir;
        cout << " Please wait ... " << endl;

        app.read_in_dir(*in_dir,
                        *check_types,
                        MwBarCode::DEFAULT_LEVEL);
    }

    if (!app.has_images())
    {
        cout << "No image files found in: " << *in_dir << endl;
        return 0;
    }


//    if (*sort_files)
//    {
//        // sort found paths according to date exif:DateTime
//
//        high_resolution_clock::time_point t1 = high_resolution_clock::now();
//        app.sort_parhs2(*no_of_threads);
//        high_resolution_clock::time_point t2 = high_resolution_clock::now();
//
//        auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
//
//        fmt::print("Sorting time measured: {:d}\n", duration);
//
//    }
//
//
//    // get all image paths in an input folder
//    MwBarCode::paths_vector found_files = app.getPaths();
//
//    // create image processing threads
//    ProcessImages process_images {*no_of_threads,
//                                  found_files,
//                                  *verbose};
//
//
//    high_resolution_clock::time_point t1 = high_resolution_clock::now();
//
//    // start processing the images in one or more threads
//    process_images.start_threads();
//    process_images.join_threads();
//
//    high_resolution_clock::time_point t2 = high_resolution_clock::now();
//
//    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
//
//    fmt::print("Time measured: {:d}\n", duration);
//
//    // get avarage values of colorsfrom the images
//    ProcessImages::out_vector out_values;
//    out_values = process_images.get_results();
//
//    // make the barcode
//    Magick::Image bar_code;
//    bar_code = app.makeBarCode(out_values);

    Image bar_code( Geometry(1600,300), Color("blue") );

    app.addDates(bar_code);


    // save it
    bar_code.write((*out_img).string());

    return 0;
};
