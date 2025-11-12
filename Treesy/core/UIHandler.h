// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#ifndef _UI_HANDLER_H
#define _UI_HANDLER_H

#include "../../PennyEngine/ui/components/ButtonListener.h"

class UIHandlerImpl : public pe::ButtonListener {
public:
    void init();
protected:
    virtual void buttonPressed(std::string buttonId);
private:
    void saveImage(std::string path);
};

class UIHandler {
public:
    static void init();

    static std::string getExportPath();
    static std::string getSavePath();
    static std::string getLoadPath();
private:
    static inline UIHandlerImpl _instance;
};

#endif