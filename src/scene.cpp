
#include "pinta/scene.h"

namespace pinta {

Scene::Scene()
{
}

Scene::~Scene()
{
    for (Mesh *mesh: meshes) {
        delete mesh;
    }
}

}
