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


    cout << "In Dir: " << *in_dir << endl;

    return 0;
}