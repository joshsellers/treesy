// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#ifndef _VISUAL_NODE_H
#define _VISUAL_NODE_H

#include <string>
#include <vector>
#include "../../PennyEngine/ui/components/TextField.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include "../../PennyEngine/core/Defines.h"

class VisualNode : public pe::TextField {
public:
    VisualNode(VisualNode* parent, float x, float y);

    void tick();
    void visualize(sf::RenderTexture& surface);

    sf::Vector2f getPosition() const;

    std::vector<s_p<VisualNode>>& getChildren();
    VisualNode* getParent();

    bool hasChildren();
    bool hasParent() const;

    bool isHovered() const;
    bool isArmed() const;

    bool hasSubscript() const;

    virtual void mouseButtonPressed(const int mx, const int my, const int button);
    virtual void mouseButtonReleased(const int mx, const int my, const int button);
    virtual void mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll);
    virtual void mouseMoved(const int mx, const int my);
    virtual void textEntered(const sf::Uint32 character); 
    
    virtual bool hasMousePriority() const;
    void releasePriority();

    bool isSelectingMovement() const;
    bool hasMovement() const;
    float getMovementLineVertex() const;
protected:
    virtual void update();
    virtual void draw(sf::RenderTexture& surface); 
private:
    virtual void drawMovementLine(sf::RenderTexture& surface);

    bool anotherNodeIsBlocking() const;

    std::vector<s_p<VisualNode>> _children;

    void addChild();

    VisualNode* _parent = nullptr;

    bool _hideInterface = true;

    void connectToParent(sf::RenderTexture& surface);

    sf::RectangleShape _plusButton;
    sf::RectangleShape _minusButton;
    bool _clickingButtons = false;

    sf::Text _subscript;
    bool _enteringSubscript = false;

    bool _selectingMovement = false;
    bool _hasMovement = false;
    s_p<VisualNode> _endPointNode = nullptr;

    float _curveAngle = 0.f;
    float _curveHeight = 0.f;

    float _movementLineVertex = 0;
};

#endif