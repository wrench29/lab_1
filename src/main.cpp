#include "scene.hpp"

int main()
{
    Scene* scene = Scene::getInstance();
    scene->loop();
    return 0;
}
