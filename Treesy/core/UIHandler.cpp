// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "UIHandler.h"
#include "../../PennyEngine/ui/UI.h"
#include "../../PennyEngine/ui/components/TextField.h"
#include "../../PennyEngine/ui/components/Panel.h"
#include "../../PennyEngine/ui/components/Button.h"
#include "../../PennyEngine/core/Util.h"
#include <Windows.h>
#include "../../PennyEngine/core/Logger.h"
#include "../visual/VisualTree.h"
#include "Settings.h"

void UIHandlerImpl::init() {
    auto subscriptMenu = pe::UI::addMenu("subscriptMenu");
    subscriptMenu->addComponent(new_s_p(pe::TextField, ("subscriptField", 50, 38, 9, 4, "")));
    subscriptMenu->addComponent(new_s_p(pe::Button, ("close_subscriptMenu", 50, 44, 6, 3, "Done", this)));
    auto subscriptPanel = new_s_p(pe::Panel, ("subscriptPanel", 50, 40, 12, 16, "Add subscript:", true));
    subscriptPanel->setTextPosition({50.f, 11.f});
    subscriptMenu->addComponent(subscriptPanel);
    subscriptPanel->attach(subscriptMenu->getComponent("subscriptField"));
    subscriptPanel->attach(subscriptMenu->getComponent("close_subscriptMenu"));

    auto mainMenu = pe::UI::addMenu("main");
    const sf::Vector2f posOffset = { -3, -3 };
    mainMenu->addComponent(new_s_p(pe::Button, ("open_settings", 10 + posOffset.x, 13 + posOffset.y, 7, 3, "Settings", this)));
    mainMenu->addComponent(new_s_p(pe::Button, ("export", 10 + posOffset.x, 18 + posOffset.y, 8, 3, "Export tree", this)));
    mainMenu->addComponent(new_s_p(pe::Button, ("exit", 10 + posOffset.x, 23 + posOffset.y, 6, 3, "Quit", this)));
    auto mainPanel = new_s_p(pe::Panel, ("mainPanel", 10 + posOffset.x, 17 + posOffset.y, 10, 21, "Treesy", true));
    mainPanel->setTextPosition({ 50.f, 14.f });
    mainMenu->addComponent(mainPanel);
    mainPanel->attach(mainMenu->getComponent("open_settings"));
    mainPanel->attach(mainMenu->getComponent("export"));
    mainPanel->attach(mainMenu->getComponent("exit"));
    mainMenu->open();
}

void UIHandler::init() {
    _instance.init();
}

void UIHandlerImpl::buttonPressed(std::string buttonId) {
    if (pe::stringStartsWith(buttonId, "close_")) {
        const auto& menu = pe::UI::getMenu(pe::splitString(buttonId, "close_")[1]);
        if (menu != nullptr) menu->close();
    } else if (pe::stringStartsWith(buttonId, "open_")) {
        const auto& menu = pe::UI::getMenu(pe::splitString(buttonId, "open_")[1]);
        if (menu != nullptr) menu->open();
    } else if (buttonId == "export") {
        const std::string path = UIHandler::getExportPath();
        saveImage(path);
    } else if (buttonId == "exit") {
        PennyEngine::stop();
    }
}

void UIHandlerImpl::saveImage(std::string path) {
    float lowestX = 9999999;
    float lowestY = 9999999;
    float highestX = 0;
    float highestY = 0;
    for (const auto& node : VisualTree::getNodes()) {
        const sf::Vector2f pos = node->getPosition();
        const sf::Vector2f size = { node->getBounds().width, node->getBounds().height };

        lowestX = std::min(pos.x, lowestX);
        lowestY = std::min(pos.y, lowestY);
        highestX = std::max(pos.x + size.x, highestX);
        highestY = std::max(std::max(pos.y + size.y, node->getMovementLineVertex()), highestY);
    }
    const sf::Vector2f size = { highestX - lowestX, highestY - lowestY };
    const sf::View view({ lowestX + size.x / 2.f, lowestY + size.y / 2.f }, size);

    sf::RenderTexture outputSurface;
    outputSurface.create(size.x, size.y);
    const sf::Texture& outputSurfaceTexture = outputSurface.getTexture();

    sf::RectangleShape bg;
    bg.setFillColor(Settings::bgColor);
    bg.setPosition(lowestX, lowestY);
    bg.setSize({(float)view.getSize().x, (float)view.getSize().y});

    outputSurface.setView(view);
    outputSurface.draw(bg);
    VisualTree::draw(outputSurface);
    outputSurface.display();

    const bool result = outputSurfaceTexture.copyToImage().saveToFile(path);

    if (!result) {
        pe::Logger::log("Failed to save: " + path);
    }
}

static std::string WcharToUtf8(const WCHAR* wideString, size_t length = 0) {
    if (length == 0)
        length = wcslen(wideString);

    if (length == 0)
        return std::string();

    std::string convertedString(WideCharToMultiByte(CP_UTF8, 0, wideString, (int)length, NULL, 0, NULL, NULL), 0);

    WideCharToMultiByte(
        CP_UTF8, 0, wideString, (int)length, &convertedString[0], (int)convertedString.size(), NULL, NULL);

    return convertedString;
}

std::string UIHandler::getExportPath() {
    OPENFILENAME ofn;

    WCHAR szFileName[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = (LPCWSTR)L"PNG Files (*.png)\0*.png\0JPG Files (*.jpg)\0*.jpg\0JPEG files (*.jpeg)\0*.jpeg\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName; 
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = (LPCWSTR)L"png";

    GetSaveFileName(&ofn);
    return WcharToUtf8(ofn.lpstrFile);
}