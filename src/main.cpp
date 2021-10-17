#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>

#include "scene.hpp"
#include "utils.hpp"

int main()
{
    Scene* scene = Scene::getInstance();
    scene->loop();
    return 0;
}
