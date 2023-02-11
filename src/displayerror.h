#ifndef PINTA_DISPLAYERROR_H
#define PINTA_DISPLAYERROR_H

#include <exception>
#include <string>

namespace pinta {

class DisplayError: public std::exception {

public:

    DisplayError(const std::string &msg);

private:

    std::string msg;

};

}

#endif