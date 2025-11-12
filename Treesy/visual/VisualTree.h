// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#ifndef _VISUAL_TREE_H
#define _VISUAL_TREE_H

#include <SFML/Graphics/Texture.hpp>
#include "VisualNode.h"
#include "../../PennyEngine/core/Defines.h"
#include "../../PennyEngine/input/KeyListener.h"
#include "../../PennyEngine/input/MouseListener.h"

class VisualTreeImpl : public pe::KeyListener, public pe::MouseListener {
public:
    VisualTreeImpl();

    void update();
    void draw(sf::RenderTexture& surface);

    s_p<VisualNode> addChild(VisualNode* parent);

    std::vector<s_p<VisualNode>> getNodes();

    void reset();

    friend class PersistenceImpl;
protected:
    virtual void mouseButtonPressed(const int mx, const int my, const int button);
    virtual void mouseButtonReleased(const int mx, const int my, const int button);
    virtual void mouseMoved(const int mx, const int my);
    virtual void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    virtual void keyPressed(sf::Keyboard::Key& key) {};
    virtual void keyReleased(sf::Keyboard::Key& key) {};
    virtual void textEntered(sf::Uint32 character);
private:
    std::vector<s_p<VisualNode>> _nodes;
    std::vector<s_p<VisualNode>> _renderNodes;

    std::vector<s_p<VisualNode>> _nodeBuffer;

    float alignNode(s_p<VisualNode> node);
    void centerNodes(s_p<VisualNode> node);
};

class VisualTree {
public:
    static void update() {
        _instance.update();
    }

    static void draw(sf::RenderTexture& surface) {
        _instance.draw(surface);
    }

    static s_p<VisualNode> addChild(VisualNode* parent) {
        return _instance.addChild(parent);
    }

    static std::vector<s_p<VisualNode>> getNodes() {
        return _instance.getNodes();
    }

    static void reset() {
        _instance.reset();
    }

    friend class PersistenceImpl;
private:
    static inline VisualTreeImpl _instance;
};

#endif