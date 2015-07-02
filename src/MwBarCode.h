//
// Created by marcin on 24/05/15.
//

#ifndef UNTITLED_NEWCPPCLASS_H
#define UNTITLED_NEWCPPCLASS_H

#include "BoostHeaders.h"
#include "MagickHeaders.h"

#include "MwImage.h"
#include "utils.h"
#include "ProcessImages.h"

#include "../ext/format.h"

using namespace std;
using namespace boost::filesystem;
using boost::optional;
using boost::none;

namespace po = boost::program_options;

class MwBarCode {

public:


    using paths_vector = vector<path>;
    using sorted_vector = vector<pair<path, time_t>>;

    bool use_only_images  {true};
    bool options_ok       {false};

    static const int  DEFAULT_LEVEL      = -1;
    static const bool DEFAULT_VERBOSE    = false;
    static const bool DEFAULT_CHECK_TYPE = true;
    static constexpr double BAR_HEIGHT_RATIO = 0.42857;

    static bool VERBOSE;

    static const vector<string> TIME_DATE_EXIF_FILEDS;


    MwBarCode(int acc, const char *avv[]);

    void
    read_in_dir(const path & in_dir,
                bool check_types = DEFAULT_CHECK_TYPE,
                int max_level = DEFAULT_LEVEL);

    template<typename T>
    optional<T> get_option(const string & opt_name) const;

    bool has_images() const {return !found_paths.empty();};


    time_t
    readImageDate(path& img_path);


    const paths_vector &
    getPaths() const {return found_paths;}

    const sorted_vector &
    getSortedPaths() const {return sorted_paths;}

    void addDates(Magick::Image& _img);


    void sort_parhs();
    void sort_parhs2();

    template<typename T>
    Magick::Image
    makeBarCode(const vector<T>& avg_pixels) const;


    void test();

    ~MwBarCode();

private:

    paths_vector found_paths;
    sorted_vector sorted_paths;
    po::variables_map vm;

    void ParseOptions(int acc, const char *avv[]);


};


#endif //UNTITLED_NEWCPPCLASS_H
