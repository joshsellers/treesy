// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#ifndef _APPEARANCE_CONFIIGS_H
#define _APPEARANCE_CONFIIGS_H

#include "ComponentAppearanceConfig.h"

namespace pe {
    inline ComponentAppearanceConfig BASE_COMPONENT_CONFIG = {
        0.09f,
        {32, 80, 2, 2},
        {32, 83, 2, 1},
        {32, 85, 2, 2},
        {35, 80, 1, 2},
        {35, 83, 1, 1},
        {35, 85, 1, 2},
        {37, 80, 2, 2},
        {37, 83, 2, 1},
        {37, 85, 2, 2}
    };

    inline ComponentAppearanceConfig BUTTON_CONFIG = BASE_COMPONENT_CONFIG;

    inline ComponentAppearanceConfig BUTTON_HOVER_CONFIG = BUTTON_CONFIG.offsetBy(16, 0);

    inline ComponentAppearanceConfig BUTTON_CLICKED_CONFIG = BUTTON_CONFIG.offsetBy(32, 0);

    inline ComponentAppearanceConfig PANEL_CONFIG = {
        0.09f,
        {48, 96, 3, 3},
        {48, 100, 3, 1},
        {48, 102, 3, 3},
        {52, 96, 1, 3},
        {52, 100, 1, 1},
        {52, 102, 1, 3},
        {54, 96, 3, 3},
        {54, 100, 3, 1},
        {54, 102, 3, 3}
    };

    inline ComponentAppearanceConfig SLIDER_HANDLE_CONFIG = BASE_COMPONENT_CONFIG;
    inline ComponentAppearanceConfig SLIDER_RAIL_CONFIG = BASE_COMPONENT_CONFIG;

    inline ComponentAppearanceConfig TEXTFIELD_CONFIG = BUTTON_CONFIG.offsetBy(0, 32);
    inline ComponentAppearanceConfig TEXTFIELD_HOVER_CONFIG = TEXTFIELD_CONFIG.offsetBy(16, 0);
    inline ComponentAppearanceConfig TEXTFIELD_ARMED_CONFIG = TEXTFIELD_CONFIG.offsetBy(32, 0);

    inline ComponentAppearanceConfig NODE_CONFIG = TEXTFIELD_CONFIG.offsetBy(0, 16);
    inline ComponentAppearanceConfig NODE_HOVER_CONFIG = TEXTFIELD_HOVER_CONFIG.offsetBy(0, 16);
    inline ComponentAppearanceConfig NODE_ARMED_CONFIG = TEXTFIELD_ARMED_CONFIG.offsetBy(0, 16);
}

#endif