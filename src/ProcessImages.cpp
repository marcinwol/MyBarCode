//
// Created by m on 5/06/15.
//

#include "ProcessImages.h"


ProcessImages::ProcessImages(
        size_t _no_of_threads, const vector<path>& _paths) :
            no_of_threads {_no_of_threads}, file_paths {_paths},
            out_values(_paths.size(), out_struct())
{

    no_of_files = file_paths.size();
}

void
ProcessImages::start_threads()
{
    for (size_t i = 0; i < no_of_threads; ++i)
    {
        //processing_threads.emplace_back(ref(execute));
        void (ProcessImages::*fun)();
        fun = &ProcessImages::execute;
        processing_threads.emplace_back(fun, this);
    }
}


void
ProcessImages::join_threads()
{
    for (thread& t: processing_threads)
    {
        t.join();
    }
}

void
ProcessImages::execute()
{

    path* image_path;

    while (i < no_of_files)
    {
        {
            lock_guard<mutex> lock(process_mutex);
            //fmt::print("{}/{}: {}", i, no_of_files, *image_path);
            image_path = &file_paths.at(i++);
            cout << i << "/" << no_of_files
                 << ": " << *image_path
                 << endl;
        }

        MwImage mw_image {*image_path};
        MwColor avg_color = mw_image.getAvgPixel();
        out_struct out_val {avg_color};

        {
            lock_guard<mutex> lock(process_mutex);
            //out_values.push_back(out_val);
            out_values.at(i-1) = out_val;
        }
    }
}