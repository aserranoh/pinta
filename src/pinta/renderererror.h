#ifndef PINTA_RENDERERERROR_H
#define PINTA_RENDERERERROR_H

#include <string>

namespace pinta {

class RendererError: std::exception {

public:

    RendererError(const std::string &msg);

private:

    std::string msg;

};

}

#endif
