// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "UIHandler.h"
#include "../../PennyEngine/ui/UI.h"
#include "../../PennyEngine/ui/components/TextField.h"
#include "../../PennyEngine/ui/components/Panel.h"
#include "../../PennyEngine/ui/components/Button.h"
#include "../../PennyEngine/core/Util.h"

void UIHandlerImpl::init() {
    auto subscriptMenu = pe::UI::addMenu("subscriptMenu");
    subscriptMenu->addComponent(new_s_p(pe::TextField, ("subscriptField", 50, 38, 9, 4, "")));
    subscriptMenu->addComponent(new_s_p(pe::Button, ("close_subscriptMenu", 50, 44, 6, 3, "Done", this)));
    auto subscriptPanel = new_s_p(pe::Panel, ("subscriptPanel", 50, 40, 12, 16, "Add subscript:"));
    subscriptPanel->setTextPosition({50.f, 11.f});
    subscriptMenu->addComponent(subscriptPanel);
    subscriptPanel->attach(subscriptMenu->getComponent("subscriptField"));
    subscriptPanel->attach(subscriptMenu->getComponent("close_subscriptMenu"));
}

void UIHandler::init() {
    _instance.init();
}

void UIHandlerImpl::buttonPressed(std::string buttonId) {
    if (pe::stringStartsWith(buttonId, "close_")) {
        pe::UI::getMenu(pe::splitString(buttonId, "close_")[1])->close();
    }
}