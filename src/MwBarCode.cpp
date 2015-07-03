#include "MwBarCode.h"

const vector<string>
MwBarCode::TIME_DATE_EXIF_FILEDS = {"exif:DateTime",
                                    "exif:DateTimeDigitized",
                                    "exif:DateTimeOriginal"};


bool MwBarCode::VERBOSE = MwBarCode::DEFAULT_VERBOSE;


MwBarCode::MwBarCode(int acc, const char *avv[])
{
    ParseOptions(acc, avv);
}

void
MwBarCode::ParseOptions(int acc, const char *avv[])
{

    po::positional_options_description p;

    p.add("in-dir", 1);
    p.add("out-file", 1);

    po::options_description desc("mwbarcode - allowed options"
                                 "\n"
                                 "Examples:\n"
                                 "mwbarcode /home/m/in_dir_with_images ./barcode.png -t 1 -v -s\n"
                                 "mwbarcode /home/m/in_dir_with_images ./barcode.png -t 4 -v -s -c\n"
    );

    path default_output_file;
    default_output_file = current_path() / path("_barcode.png");

    desc.add_options()
            ("help,h", po::value<bool>()->implicit_value(true),
             "produce help message")
            ("in-dir,i",  po::value<path>()->default_value(current_path()),
             "input folder")
            ("threads,t",  po::value<size_t>()->default_value(1),
             "Numper of image processing threads: 1 or more")
            ("check-types,c",  po::bool_switch()->default_value(true),
             "Do NOT check if a files are actually images that we can use.")
            ("out-file,o",  po::value<path>()->default_value(default_output_file),
             "ouput image file that will be bar code generated")
            ("out-format,T", po::value<string>()->default_value("TIFF"),
             "output image format (e.g. TIFF, PNG, JPEG)")
            ("sort-files,s", po::bool_switch()->default_value(false),
             "sort image files by date of creation")
            ("verbose,v", po::bool_switch()->default_value(false),
             "verbose output");


    po::store(po::command_line_parser(acc, avv)
                      .options(desc)
                      .positional(p)
                      .run(), vm);

    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";

        options_ok = false;
        return;
    }

    options_ok = true;

}


template<typename T>
optional<T>
MwBarCode::get_option(const string & opt_name) const
{

    if (!vm.count(opt_name))
    {
        return none;
    }

    return vm[opt_name].as<T>();
}


// explicit instantiations of get template function
template optional<string>
        MwBarCode::get_option<string>(const string & opt_name) const;

template optional<double>
        MwBarCode::get_option<double>(const string & opt_name) const;

template optional<bool>
        MwBarCode::get_option<bool>(const string & opt_name) const;

template optional<path>
        MwBarCode::get_option<path>(const string & opt_name) const;

template optional<size_t>
        MwBarCode::get_option<size_t>(const string & opt_name) const;

void
MwBarCode::read_in_dir(const path & in_dir, bool check_types, int max_level)
{

    bool verbose = MwBarCode::VERBOSE;

    paths_vector  all_found_paths {};
    all_found_paths = mw::fs::get_all_paths_fts2(in_dir, max_level, verbose);

    // we want only images, so filter out all paths that belong
    // to non-image file types.
    if (use_only_images && check_types)
    {
        size_t no_of_paths = all_found_paths.size();

        for (size_t i = 0; i< no_of_paths; ++i)
        {
            const path& _path = all_found_paths.at(i);

            if (verbose) {
                cout << "Check if image type of interest: ";
                cout << i << "/" << no_of_paths << ": " << _path << " ... ";
            }

            if (MwImage::fast_is_image(_path))
            {
                if (verbose) {
                    cout << " - this is good.";
                }

                found_paths.push_back(_path);
            }
            else
            {
                if (verbose) {
                    cout << " - dont want this.";
                }
            }

            if (verbose) {
                cout << endl;
            }

        }

    }
    else
    {
        found_paths = all_found_paths;
    }

}



void
MwBarCode::readImageDate(path img_path, time_t& out_t, mutex& mtx,
                         size_t path_idx, size_t no_of_paths)
{
    MwImage mwi {img_path, MwImage::DO_NOT_READ_IMG};

    mwi.ping();
    mwi.readProperties();

    time_t c_t {0};

    const MwImage::properties_map &props = mwi.getProperties();

    MwImage::properties_map::const_iterator it;


    stringstream ss;

    if (VERBOSE)
    {
        ss << "Read date of image: ";
        ss << path_idx << "/" << no_of_paths << endl;
    }


    for (string field: TIME_DATE_EXIF_FILEDS)
    {
        it = props.find(field);

        if (it != props.end())
        {
            string datetime = it->second;

            if (datetime.empty())
            {
                continue;
            }

            // change string into time
            tm t{0};
            strptime(datetime.c_str(), "%Y:%m:%d %H:%M:%S", &t);

            c_t = mktime(&t);

            if (VERBOSE)
            {
                string time_s = ctime(&c_t);
                time_s.erase(time_s.size() - 1);

                ss << "\t" << time_s << endl;

            }

            break;
        }
        else
        {
            if (VERBOSE)
            {
                    ss << "\tField " << field << " not found in "
                         << img_path << endl;
            }
        }
    }

    if (VERBOSE)
    {
        lock_guard<mutex> lock(mtx);
        cout << ss.str() << endl;
    }


    out_t = c_t;
}

void
MwBarCode::sort_parhs2(size_t no_of_threads)
{

    vector<thread> processing_threads;

    // mutex to be shared among the threads
    mutex process_mutex;


    // split vector into chunks of size of no_of_threads
    vector<paths_vector> split_paths
            = mw::chunker(found_paths, no_of_threads);

    size_t path_idx {0};
    size_t no_of_paths = found_paths.size();


    // process each chunk
    for (paths_vector& paths_sub_vector: split_paths)
    {
        vector<thread> processing_threads;

        size_t no_subpaths = paths_sub_vector.size();

        // here we will store dates obtained
        // from MwBarCode::readImageDate
        vector<time_t> read_dates(no_subpaths, 0);

        // start threads for each chunk
        for (size_t i = 0; i < no_subpaths; ++i)
        {
            processing_threads.emplace_back(&MwBarCode::readImageDate,
                                            this,
                                            paths_sub_vector.at(i),
                                            ref(read_dates.at(i)),
                                            ref(process_mutex),
                                            ++path_idx,
                                            no_of_paths
            );

        }

        // wait for each thread to finish
        for (size_t i = 0; i < no_subpaths; ++i)
        {
            processing_threads.at(i).join();
        }

        // save paths with dates found
        for (size_t i = 0; i < no_subpaths; ++i)
        {
            time_t date = read_dates.at(i);

            if (date == 0)
                continue;

            sorted_paths.push_back(
                    make_pair(paths_sub_vector.at(i), date)
            );
        }

    }

    // now sort the paths according to date
    sortByDate(sorted_paths);


    // clear found paths as they are unsorted
    // and populate it with paths in order
    found_paths.clear();

    for (const pair<path, time_t>& sp: sorted_paths)
    {
        found_paths.push_back(sp.first);
    }


}

void
MwBarCode::sort_parhs()
{

    size_t no_of_paths = found_paths.size();

    MwImage::properties_map::const_iterator it;

    for (size_t i = 0; i < no_of_paths; ++i)
    {
        const path &_path = found_paths.at(i);

        MwImage mwi {_path, MwImage::DO_NOT_READ_IMG};

        mwi.ping();
        mwi.readProperties();

        const MwImage::properties_map &props = mwi.getProperties();

        cout << "Read date of image: ";
        cout << i << "/" << no_of_paths << endl;

        for (string field: TIME_DATE_EXIF_FILEDS)
        {

            it = props.find(field);

            if (it != props.end())
            {
                string datetime = it->second;

                if (datetime.empty())
                {
                    continue;
                }

                // change string into time
                tm t{0};
                strptime(datetime.c_str(), "%Y:%m:%d %H:%M:%S", &t);

                time_t c_t = mktime(&t);

                if (VERBOSE)
                {
                    string time_s = ctime(&c_t);
                    time_s.erase(time_s.size() - 1);

                    cout << "\t" << time_s << endl;
                }


                sorted_paths.push_back(make_pair(_path, c_t));

                break;
            }
            else
            {
                if (VERBOSE)
                {
                    cout << "\tField " << field << " not found in " << _path << endl;
                }
            }
        }

    }


    // now sort the paths according to date
    sortByDate(sorted_paths);


    // clear found paths as they are unsorted
    // and pupulate it with paths in ord
    found_paths.clear();

    for (const pair<path, time_t>& sp: sorted_paths)
    {
        found_paths.push_back(sp.first);
    }


}



void
MwBarCode::sortByDate(sorted_vector& _paths)
{
    sort(_paths.begin( ),_paths.end( ),
         [](const path_date_pair& lhs, const path_date_pair& rhs)
         {
             return lhs.second < rhs.second;
         });
}


template<typename T>
Magick::Image
MwBarCode::makeBarCode(const vector<T>& avg_pixels) const
{
    size_t columns {avg_pixels.size()};
    size_t rows    {static_cast<size_t>(columns * BAR_HEIGHT_RATIO)};

    // Create base image
    Magick::Image image(Magick::Geometry(columns, rows), "white");

    // Set the image type to TrueColor DirectClass representation.
    image.type(Magick::TrueColorType);

    // Ensure that there is only one reference to underlying image
    // If this is not done, then image pixels will not be modified.
    image.modifyImage();

    // Allocate pixel view
    Magick::Pixels view(image);

    MagickCore::PixelPacket* pixels = view.get(0, 0, columns, rows);

    for (size_t ci = 0; ci < columns; ++ci)
    {
        MwColor color = MwColor(avg_pixels.at(ci));

        for (size_t ri = 0; ri < rows; ++ri)
        {
            *(pixels + ri*columns + ci ) = color;
        }
    }

    view.sync();

    return image;

}


void
MwBarCode::addDates(Magick::Image& img)
{

    size_t columns {img.columns()};
    size_t rows    {img.rows()};


    // set font size proportionally to image height
    unsigned font_size = static_cast<unsigned>(rows * 0.1);

    img.strokeWidth(1);
    img.font("Courier");
    img.fontPointsize(font_size);
    img.strokeColor("white");
    img.fillColor("black");


    // measure the font width
    // we use Courier so each date should be of equail width.
    Magick::TypeMetric typeMetrick;
    img.fontTypeMetrics("2015:12:29", &typeMetrick);

    unsigned text_width = typeMetrick.textWidth();

    unsigned no_of_dates = static_cast<unsigned>(columns / text_width);

    cout << "no_of_dates" << no_of_dates << endl;

    // use less than maximum number of dates possible
    if (no_of_dates > 3)
        no_of_dates -= 2;


    // distance between each date
    unsigned x_step    = static_cast<unsigned>(columns / no_of_dates);



    // how to offset the dates so that they are equally spaced
    // on the image borders
    unsigned x_offset  = (x_step - text_width) / 2 - 3;

    cout << "x_offset" << x_offset << endl;
    std::list<Magick::Drawable> to_draw;

    char buffer[40];


    for(size_t date_i = 0; date_i < no_of_dates; ++date_i)
    {
        unsigned x = date_i * x_step + x_offset;

        time_t timestamp = sorted_paths.at(x).second;

        strftime (buffer, 40, "%Y:%m:%d", localtime(&timestamp));

        to_draw.push_back(Magick::DrawableText(x, rows - 5, buffer));
    }


//    for(size_t date_i = 0; date_i < no_of_dates; ++date_i)
//    {
//        unsigned x = date_i * x_step + x_offset;
//
//        to_draw.push_back(Magick::DrawableText(x, rows - 5,
//                                               "2015:12:29"));
//    }

    img.draw(to_draw);

}



template Magick::Image
MwBarCode::makeBarCode(const vector<MwColor>& avg_pixels) const;

template Magick::Image
MwBarCode::makeBarCode(const vector<ProcessImages::out_struct>& avg_pixels) const;


void
MwBarCode::test()
{

   // string in_img {"/home/m/Desktop/Untitled.png"};
    //string in_img {"/home/m/Desktop/test_modified.png"};
    string in_img {"/mnt/e/ImageDatabases/Martin_OMSOK_xrays/04a_cropped/032_POST_947415_DPI253_d128px.tiff"};

   // Magick::Image mImg {in_img};
    MwImage mImg {in_img};


    MwColor c {mImg.getPixel(157, 103)};
    MwColor c1 {mImg.getPixel(157, 103)};


    mImg.getAvgPixel();

    cout << "testing" << endl;
};

MwBarCode::~MwBarCode()
{

}

