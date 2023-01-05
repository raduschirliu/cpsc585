#include "game/GameApp.h"

using glm::ivec2;

GameApp::GameApp()
{
}

void GameApp::Init()
{
    GetWindow().SetSize(ivec2(1280, 720));
}