//
// Created by m on 5/06/15.
//

#include "ProcessImages.h"


ProcessImages::ProcessImages(
        size_t _no_of_threads, const vector<path>& _paths) :
            no_of_threads {_no_of_threads}, file_paths {_paths}
{

    no_of_files = file_paths.size();
}


bool
ProcessImages::execute()
{

    path* image_path;

    while (i < no_of_files)
    {
        {
            lock_guard<mutex> lock(process_mutex);
            image_path = &file_paths.at(i++);
        }

        MwImage mw_image {*image_path};
        MwColor avg_color = mw_image.getAvgPixel();

        {
            lock_guard<mutex> lock(process_mutex);
            avg_pixels.push_back(avg_color);
        }
    }

}