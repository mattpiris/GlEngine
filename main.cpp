#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "core/Game.h"
constexpr auto WIN_WIDTH = 1920;
constexpr auto WIN_HEIGHT = 1080;

int main()
{
    Game game;
    game.init_and_createWindow(WIN_WIDTH, WIN_HEIGHT);
    
    return 0;
}