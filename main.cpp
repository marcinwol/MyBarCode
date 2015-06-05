#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include "src/MwBarCode.h"

using namespace Magick;


namespace {


    atomic_size_t i {0};

    MwBarCode::paths_vector found_files ;
    vector<MwColor> avg_pixels;

    mutex g_mutex;


    void process_files()
    {

        while (i < found_files.size() )
        {
            path a_file;

            {
                std::lock_guard<mutex> lock3(g_mutex);
                a_file = found_files.at(++i);
                cout << i << endl;
            }

            //}

            // fmt::print("{:d}/{:d}: {:s}\n", i, found_files.size(), a_file.string());

            MwImage mwi {a_file};

            {
                std::lock_guard<mutex> lock2(g_mutex);
                avg_pixels.emplace_back(mwi.getAvgPixel());
            }
        }

    }

}

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
    found_files = app.getPaths();

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

    vector<thread> thrds;

    for (size_t ti = 0; ti <= 10; ++ti)
    {
        thrds.push_back(thread(process_files));
    }


    for (thread& t: thrds)
    {
        t.join();
    }


    Magick::Image bar_code;

    bar_code = app.makeBarCode(avg_pixels);

    bar_code.write("/home/marcin/Desktop/out.png");

   // app.test();

    return 0;
};
