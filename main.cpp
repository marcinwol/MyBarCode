#include <iostream>
#include <thread>


#include "src/MwBarCode.h"
#include "src/ProcessImages.h"

using namespace Magick;



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

    process_images.start_threads();
    process_images.join_threads();

//
//    size_t no_of_imgs = found_files.size();
//
//    for (const path& a_file: found_files)
//    {
//
//        fmt::print("{:d}/{:d}: {:s}\n", i, no_of_imgs, a_file.string());
//
//        MwImage mwi {a_file};
//
//        avg_pixels.emplace_back(mwi.getAvgPixel());
//
//        ++i;
//
//    }
//
//    vector<thread> thrds;
//
//    for (size_t ti = 0; ti <= 10; ++ti)
//    {
//        thrds.push_back(thread(process_files));
//    }
//
//
//    for (thread& t: thrds)
//    {
//        t.join();
//    }


//    Magick::Image bar_code;
//
//    bar_code = app.makeBarCode(avg_pixels);

   // bar_code.write("/home/m/Desktop/out.png");

   // app.test();

    return 0;
};
