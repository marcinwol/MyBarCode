#include <iostream>

#include "src/MwBarCode.h"

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
    MwBarCode::paths_vector found_files {app.getPaths()};

    // vector to store avarage color of each image
    vector<MwColor> avg_pixels;

    for (const path& a_file: found_files)
    {
        cout << a_file << endl;

        MwImage mwi {a_file};

        avg_pixels.emplace_back(mwi.getAvgPixel());

    }

    Magick::Image bar_code;

    bar_code = app.makeBarCode(avg_pixels);

    bar_code.write("/home/marcin/Desktop/out.png");

    app.test();

    return 0;
};
