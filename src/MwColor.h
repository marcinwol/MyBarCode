//
// Created by m on 25/05/15.
//

#ifndef MWBARCODE_MWCOLOR_H
#define MWBARCODE_MWCOLOR_H

#include "MagickHeaders.h"

class MwColor: public Magick::ColorRGB
{
public:
    using Magick::ColorRGB::ColorRGB;

    // Assignment operator from base class
    MwColor & operator= ( const Magick::Color & color_ )
    {
        Magick::ColorRGB::operator=(color_);
        return *this;
    }

};


#endif //MWBARCODE_MWCOLOR_H
