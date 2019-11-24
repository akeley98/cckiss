#ifndef CCKISS_EXAMPLE_GADGET_HPP
#define CCKISS_EXAMPLE_GADGET_HPP

#include <ctype.h>
#include <iosfwd>
#include <string>
#include "config.h"

struct Gadget
{
    int number;
    std::string name;
};

std::ostream& operator << (std::ostream& os, const Gadget& g)
{
    
    if (config_shout) {
        os << "GADGET #" << g.number << " NAMED ";
        for (char c : g.name) os << char(toupper(c));
    }
    else {
        os << "Gadget #" << g.number << " named "  << g.name;
    }
    return os;
}

#endif /* !CCKISS_EXAMPLE_GADGET_HPP */
