// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "ProgramManager.h"
#include "../visual/VisualTree.h"
#include "../../PennyEngine/PennyEngine.h"
#include "../../PennyEngine/core/Logger.h"
#include "UIHandler.h"
#include "../../PennyEngine/ui/UI.h"
#include "Settings.h"
#include "../visual/Line.h"

ProgramManager::ProgramManager() {
    PennyEngine::addInputListener(this);
}

void ProgramManager::init() {
    VisualTree::addChild(nullptr);

    UIHandler::init();
}

void ProgramManager::update() {
    VisualTree::update();
}

void ProgramManager::draw(sf::RenderTexture& surface) {
    sf::RectangleShape bg;
    bg.setSize(PennyEngine::getCamera().getSize());
    const sf::Vector2f cameraPos = PennyEngine::getCamera().getCenter();
    bg.setPosition(cameraPos.x - bg.getSize().x / 2.f, cameraPos.y - bg.getSize().y / 2.f);
    bg.setFillColor(Settings::bgColor);
    surface.draw(bg);

    VisualTree::draw(surface);
}

void ProgramManager::drawUI(sf::RenderTexture& surface) {
}

void ProgramManager::onShutdown() {
}

void ProgramManager::buttonPressed(std::string buttonId) {

}

void ProgramManager::sliderMoved(std::string sliderId, float value) {

}

void ProgramManager::keyPressed(sf::Keyboard::Key& key) {
}

void ProgramManager::keyReleased(sf::Keyboard::Key& key) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && key == sf::Keyboard::V) {
        for (const auto& node : VisualTree::getNodes()) {
            if (node->isActive() && node->isArmed()) {
                node->getText().setString(node->getText().getString() + sf::Clipboard::getString());
                break;
            }
        }
    }
}

static sf::Vector2f mapMouseCoordinates(const int mx, const int my) {
    const sf::Vector2i mouseCoords(mx, my);

    return PennyEngine::getWindow().mapPixelToCoords(mouseCoords, PennyEngine::getCamera());
}

void ProgramManager::mouseButtonPressed(const int mx, const int my, const int button) {
    for (const auto& node : VisualTree::getNodes()) {
        if (node->isActive() && node->getBounds().contains(mapMouseCoordinates(mx, my))) {
            _clickedIntoNode = true;
            return;
        }
    }

    for (const auto& menu : pe::UI::getMenus()) {
        if (menu->isActive()) {
            for (const auto& component : menu->getComponents()) {
                if (component->isActive() && component->getBounds().contains(mx, my)) {
                    _clickedIntoNode = true;
                    return;
                }
            }
        }
    }
}

void ProgramManager::mouseButtonReleased(const int mx, const int my, const int button) {
    _clickedIntoNode = false;
}

void ProgramManager::mouseMoved(const int mx, const int my) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !_clickedIntoNode) {
        const sf::Vector2f viewSize = PennyEngine::getCamera().getSize();
        const sf::Vector2f res((float)PennyEngine::getRenderResolution().width, (float)PennyEngine::getRenderResolution().height);
        const sf::Vector2f scale = { viewSize.x / res.x, viewSize.y / res.y };

        const sf::Vector2f mPos = { (float)mx, (float)my };
        const sf::Vector2f clickPos = { (float)_clickPos.x, (float)_clickPos.y};
        const sf::Vector2f delta = clickPos - mPos;
        PennyEngine::getCamera().move(delta.x * scale.x, delta.y * scale.y);
    }

    if (!_clickedIntoNode) _clickPos = { mx, my };
}

void ProgramManager::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
        for (const auto& node : VisualTree::getNodes()) {
            if (node->isActive() && node->isSelectingMovement()) return;
        }
    }

    const float factor = (1.f / ((mouseWheelScroll.delta * 8.f + 100.f) / 100.f));
    PennyEngine::getCamera().zoom(factor);
}
