//
// Created by marcin on 24/05/15.
//

#ifndef UNTITLED_NEWCPPCLASS_H
#define UNTITLED_NEWCPPCLASS_H

#include "BoostHeaders.h"
#include "MagickHeaders.h"

#include "MwImage.h"
#include "utils.h"

#include "../ext/format.h"

using namespace std;
using namespace boost::filesystem;
using boost::optional;
using boost::none;

namespace po = boost::program_options;

class MwBarCode {

public:

    using paths_vector = vector<path>;

    bool use_only_images  {true};
    bool options_ok       {false};


    MwBarCode(int acc, const char *avv[]);

    void
    read_in_dir(const path & in_dir, int max_level = -1, bool verbose = true);

    template<typename T>
    optional<T> get_option(const string & opt_name) const;


    const paths_vector &
    getPaths() const {return found_paths;}

    Magick::Image makeBarCode(const vector<MwColor>& avg_pixels) const;


    void test();

    ~MwBarCode();

private:

    paths_vector found_paths;
    po::variables_map vm;

    void ParseOptions(int acc, const char *avv[]);
};


#endif //UNTITLED_NEWCPPCLASS_H
