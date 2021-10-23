#include "scene.hpp"

int main()
{
    Scene* scene = Scene::getInstance();
    scene->loop();
    delete scene;
    return 0;
}
