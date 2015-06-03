#include "MwBarCode.h"


MwBarCode::MwBarCode(int acc, const char *avv[])
{
    ParseOptions(acc, avv);
}

void
MwBarCode::ParseOptions(int acc, const char *avv[])
{

    po::positional_options_description p;
    p.add("in-dir", -1);

    po::options_description desc("mwdcmconverter - allowed options"
                                         "\n"
                                         "Examples\n"
                                         "mwdcmconverter ~/Desktop/test2/ -o ~/Desktop/tiffs --append-dpi --append-distance=12.8\n"
                                         ""
    );

    desc.add_options()
            ("help,h", po::value<bool>()->implicit_value(true),
             "produce help message")
            ("in-dir,i",  po::value<path>()->default_value(current_path()),
             "input folder")
            ("out-format,T", po::value<string>()->default_value("TIFF"),
             "output image format (e.g. TIFF, PNG, JPEG)")
            ("append-dpi", po::bool_switch()->default_value(false),
             "append DPI to the output filename")
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

void
MwBarCode::read_in_dir(const path & in_dir, int max_level, bool verbose)
{
    paths_vector  all_found_paths {};
    all_found_paths = mw::fs::get_all_paths_fts2(in_dir, max_level, verbose);

    // we want only images, so filter out all paths that belong
    // to non-image file types.
    if (use_only_images)
    {
        paths_vector::const_iterator it_begin = all_found_paths.begin();
        paths_vector::const_iterator it_end   = all_found_paths.end();

        auto is_image_lmd =
                [](const path& a_path) {return MwImage::fast_is_image(a_path);};

        copy_if(it_begin, it_end, back_inserter(found_paths), is_image_lmd);
    }
    else
    {
        found_paths = all_found_paths;
    }

}

Magick::Image
MwBarCode::makeBarCode(const vector<MwColor>& avg_pixels) const
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
        MwColor color {avg_pixels.at(ci)};

        for (size_t ri = 0; ri < rows; ++ri)
        {
            *(pixels + ri*columns + ci ) = color;
        }
    }

    view.sync();

    return image;


}


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

