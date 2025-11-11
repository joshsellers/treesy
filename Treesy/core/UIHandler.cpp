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
    mainMenu->addComponent(new_s_p(pe::Button, ("open_settings", 10, 13, 7, 3, "Settings", this)));
    mainMenu->addComponent(new_s_p(pe::Button, ("export", 10, 18, 8, 3, "Export tree", this)));
    mainMenu->addComponent(new_s_p(pe::Button, ("exit", 10, 23, 6, 3, "Quit", this)));
    auto mainPanel = new_s_p(pe::Panel, ("mainPanel", 10, 17, 10, 21, "Treesy", true));
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
        pe::Logger::log(path);
        // do something with the path
    } else if (buttonId == "exit") {
        PennyEngine::stop();
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