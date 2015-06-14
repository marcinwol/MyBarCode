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

    po::options_description desc("mwdcmconverter - allowed options"
                                 "\n"
                                 "Examples\n"
                                 "mwdcmconverter ~/Desktop/test2/ -o ~/Desktop/tiffs --append-dpi --append-distance=12.8\n"
                                 ""
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
            ("check-types,c",  po::bool_switch()->default_value(false),
             "Check if a files are actually images that we can use.")
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
MwBarCode::read_in_dir(const path & in_dir, bool check_types,
                       int max_level, bool verbose)
{
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
MwBarCode::sort_parhs()
{

    size_t no_of_paths = found_paths.size();

    MwImage::properties_map::const_iterator it;

    map<time_t, path> sorted_paths;

    for (size_t i = 0; i < no_of_paths; ++i)
    {
        const path &_path = found_paths.at(i);

        MwImage mwi {_path, MwImage::DO_NOT_READ_IMG};

        mwi.ping();
        mwi.readProperties();

        const MwImage::properties_map &props = mwi.getProperties();

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


                if (VERBOSE)
                {
                    cout << "DateTime: " << datetime << " .. read as ";
                }


                // change string into time
                tm t{0};
                strptime(datetime.c_str(), "%Y:%m:%d %H:%M:%S", &t);

                time_t c_t = mktime(&t);

                if (VERBOSE)
                {
                    string time_s = ctime(&c_t);
                    time_s.erase(time_s.size() - 1);

                    cout << time_s << endl;
                }

                sorted_paths.insert({c_t, _path});

                break;
            }
            else
            {
                if (VERBOSE)
                {
                    cout << "Field " << field << " not found in " << _path << endl;
                }
            }
        }

    }

//    for (const pair<time_t, path>& sp: sorted_paths)
//    {
//        cout << ctime(&(sp.first)) <<": " << sp.second << endl;
//    }


    // clear found paths as they are unsorted
    // and pupulate it with paths in ord
    found_paths.clear();

    for (const pair<time_t, path>& sp: sorted_paths)
    {
        found_paths.push_back(sp.second);
    }


}

template<typename T>
Magick::Image
MwBarCode::makeBarCode(const vector<T>& avg_pixels) const
{
    size_t columns {avg_pixels.size()};
    size_t rows    {200};

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

