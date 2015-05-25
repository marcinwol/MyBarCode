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
    found_paths =  mw::fs::get_all_paths_fts2(in_dir, max_level, verbose);
}


void
MwBarCode::test()
{

    string in_img {"/home/m/Desktop/test_modified.png"};

   // Magick::Image mImg {in_img};
    MwImage mImg {in_img};

  //  mImg.read();
    cout << "testing" << endl;
}

MwBarCode::~MwBarCode()
{

}

