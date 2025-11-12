// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#define NO_CONSOLE_ON_STARTUP

#include "../../PennyEngine/PennyEngine.h"
#include "ProgramManager.h"

int main() {
    PennyEngine::setAppName("Treesy");
    PennyEngine::setWindowTitle(PennyEngine::getAppName());
    PennyEngine::setFont("res/font.ttf");
    PennyEngine::setAppIcon("res/icon.png");

    PennyEngine::setFullscreen(false);
    PennyEngine::setDisplayResolution({ (int)(sf::VideoMode::getDesktopMode().width * 0.9f), (int)(sf::VideoMode::getDesktopMode().height * 0.8f) });
    PennyEngine::setRenderResolution(PennyEngine::getDisplayResolution());
    PennyEngine::setFramerateLimit(60);

    ProgramManager programManager;

    PennyEngine::start(&programManager);
}