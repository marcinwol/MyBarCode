//
// Created by m on 25/05/15.
//

#ifndef MWBARCODE_MWCOLOR_H
#define MWBARCODE_MWCOLOR_H

#include <sstream>

#include "MagickHeaders.h"

class MwColor: public Magick::ColorRGB
{
public:
    using Magick::ColorRGB::ColorRGB;

    // Assignment operator from base class
    MwColor & operator= ( const Magick::Color& color_ )
    {
        Magick::ColorRGB::operator=(color_);
        return *this;
    }

    operator std::string() const;
    MwColor operator+(const MwColor& color_) const;

private:
    friend std::ostream& operator<<(std::ostream& strm, const MwColor& a);

};


#endif //MWBARCODE_MWCOLOR_H
