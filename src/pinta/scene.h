#ifndef PINTA_SCENE_H
#define PINTA_SCENE_H

#include <list>

#include "pinta/mesh.h"

namespace pinta {

class Scene {

public:

    Scene();
    ~Scene();

    inline void addMesh(Mesh *mesh) {meshes.push_back(mesh);}
    inline const std::list<Mesh *> & getMeshes() const {return meshes;}

private:

    std::list<Mesh *> meshes;

};

}

#endif
