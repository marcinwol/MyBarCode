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
    ProcessImages process_images {4, found_files};

    process_images.start_threads();
    process_images.join_threads();

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
