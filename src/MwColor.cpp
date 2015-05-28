//
// Created by m on 25/05/15.
//

#include "MwColor.h"



MwColor::operator std::string() const
{

    std::stringstream ss;

    ss << red();
    ss << ",";
    ss << green();
    ss << ",";
    ss << blue();

    return ss.str();
}

std::ostream &
operator<<(std::ostream& strm, const MwColor& a) {
    return strm << std::string(a);
}

MwColor
MwColor::operator+ (const MwColor& color_) const {

    MwColor mwc;

    mwc.red(this->red() + color_.red());
    mwc.green(this->green() + color_.green());
    mwc.blue(this->blue() + color_.blue());

    return mwc;
}
