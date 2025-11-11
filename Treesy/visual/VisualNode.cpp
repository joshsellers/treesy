// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "VisualNode.h"
#include "../../PennyEngine/PennyEngine.h"
#include "../../PennyEngine/ui/UI.h"
#include "VisualTree.h"
#include "../../PennyEngine/core/Logger.h"
#include "Line.h"
#include "../core/Settings.h"


VisualNode::VisualNode(VisualNode* parent, float x, float y) : TextField(pe::generateUID(), x, y, 3, 5, "", "XP") {
    _parent = parent;
    show();
    _fieldText.setFillColor(Settings::nonTermColor);
    _fieldText.setCharacterSize(pe::UI::percentToScreenWidth(2.5f));

    _subscript.setFont(PennyEngine::getFont());
    _subscript.setFillColor(Settings::nonTermColor);
    _subscript.setCharacterSize(pe::UI::percentToScreenWidth(1.75f));
    _subscript.setString("");

    constructShapes();

    const float buttonSize = pe::UI::percentToScreenWidth(1.25f);
    _plusButton.setSize({ buttonSize, buttonSize });
    _plusButton.setPosition(_pos.x + getBounds().width - _plusButton.getSize().x, _pos.y + getBounds().height / 2.f - _plusButton.getSize().y);
    _plusButton.setTexture(pe::UI::getSpriteSheet().get());
    _plusButton.setTextureRect({
        0, 0, 16, 16    
    });

    _minusButton.setSize({ buttonSize, buttonSize });
    _minusButton.setPosition(_pos.x + _minusButton.getSize().x, _pos.y + getBounds().height / 2.f - _minusButton.getSize().y);
    _minusButton.setTexture(pe::UI::getSpriteSheet().get()); 
    _minusButton.setTextureRect({
        16, 0, 16, 16
    });
}

void VisualNode::tick() {
    update();
}

void VisualNode::visualize(sf::RenderTexture& surface) {
    if (!_hideInterface) {
        render(surface);
    } else {
        constructShapes();

        alignText();
        surface.draw(_text);

        draw(surface);
    }
}

sf::Vector2f VisualNode::getPosition() const {
    return { getBounds().left, getBounds().top };
}


void VisualNode::addChild() {
    if (!hasChildren() && hasParent() && getParent()->getChildren().size() == 1) move({ 0, pe::UI::percentToScreenHeight(Settings::termVerticalDistance) });
    const auto& child = VisualTree::addChild(this);
    _children.push_back(child);
}

void VisualNode::connectToParent(sf::RenderTexture& surface) {
    if (_parent != nullptr && (Settings::showTermLines || hasChildren() || getParent()->getChildren().size() > 1 || hasSubscript())) {
        const Line line(
            {_parent->getPosition().x + _parent->getBounds().width / 2.f, _parent->getPosition().y + _parent->getBounds().height}, 
            {getPosition().x + getBounds().width / 2.f, getPosition().y },
            4.f
        );
        surface.draw(line);
    }
}

void VisualNode::update() {
    bool preferParent = false;
    if (hasParent() && !_parent->isActive()) hide();
    else if (hasParent() && _parent->isArmed()) preferParent = true;

    if (!_isSelected && _lastSelected) _isArmed = false;
    _lastSelected = _isSelected;

    if (((getBounds().contains(_mPos.x, _mPos.y) && !_mouseDown) || _isSelected) && !_isArmed && !preferParent && !isSelectingMovement()) {
        setAppearance(pe::NODE_HOVER_CONFIG);
        _hideInterface = false;
    } else if ((!_mouseDown || !getBounds().contains(_mPos.x, _mPos.y)) && !_isArmed) {
        setAppearance(pe::NODE_CONFIG); // make this look better
        _hideInterface = !isSelectingMovement();
    } else if (_isArmed) {
        setAppearance(pe::NODE_ARMED_CONFIG);
        _hideInterface = false;
    }

    _size.x = std::max(_minWidth, _fieldText.getGlobalBounds().width + _padding * 2 + _subscript.getGlobalBounds().width);
    _size.y = std::max(_minHeight, _fieldText.getGlobalBounds().height);

    if (_autoCenter) {
        _pos.x = _origin.x - _size.x / 2.f - _padding;
        _pos.y = _origin.y - _size.y / 2.f;

        if (hasSubscript()) _pos.x += _subscript.getGlobalBounds().width / 2.f;
    }

    _children.erase(std::remove_if(_children.begin(), _children.end(), [](s_p<VisualNode> node) {return !node->isActive(); }), _children.end());

    if (hasParent()) {
        const float dist = getPosition().y - _parent->getPosition().y;
        if (dist <= pe::UI::percentToScreenHeight(Settings::nontermVerticalDistance)) {
            if (hasChildren() || getParent()->getChildren().size() > 1 || hasSubscript()) {
                move({ 0, pe::UI::percentToScreenHeight(Settings::termVerticalDistance) });
            }
        } else if (dist >= pe::UI::percentToScreenHeight(Settings::nontermVerticalDistance)) {
            if (!hasChildren() && getParent()->getChildren().size() == 1 && !hasSubscript()) {
                move({ 0, -pe::UI::percentToScreenHeight(Settings::termVerticalDistance) });
            }
        }
    }

    if (_enteringSubscript) {
        const auto& menu = pe::UI::getMenu("subscriptMenu");
        if (!menu->isActive()) _enteringSubscript = false;
        else _subscript.setString(menu->getComponent("subscriptField")->getText().getString());
    }
}

void VisualNode::draw(sf::RenderTexture& surface) {
    if (!hasChildren() && !hasSubscript()) _fieldText.setFillColor(Settings::termColor);
    else _fieldText.setFillColor(Settings::nonTermColor);

    _fieldText.setOrigin(_fieldText.getLocalBounds().width / 2.f + _fieldText.getLocalBounds().left, _fieldText.getLocalBounds().height / 2.f + _fieldText.getLocalBounds().top);

    const sf::FloatRect bounds = getBounds();
    const float width = bounds.width;
    const float height = bounds.height;
    _fieldText.setPosition(
        bounds.left + (width / 2.f) - (hasSubscript() ? _subscript.getGlobalBounds().width / 2.f : 0),
        bounds.top + (height / 2.f)
    );

    surface.draw(_fieldText);

    if (_isArmed) {
        sf::Text cursor;
        cursor.setString(" |");
        cursor.setFont(PennyEngine::getFont());
        cursor.setCharacterSize(_fieldText.getCharacterSize() + pe::UI::percentToScreenWidth(0.5f));
        cursor.setFillColor(_fieldText.getFillColor());
        cursor.setOrigin(cursor.getLocalBounds().width / 2.f + cursor.getLocalBounds().left, cursor.getLocalBounds().height / 2.f + cursor.getLocalBounds().top);
        cursor.setPosition(_fieldText.getPosition().x + _fieldText.getGlobalBounds().width / 2.f, _fieldText.getPosition().y);
        constexpr unsigned int blinkRate = 24;
        if ((_cursorBlinkTimer / blinkRate) % 2) surface.draw(cursor);

        _cursorBlinkTimer++;
    }

    if (hasSubscript()) {
        const float subsVertSpacing = pe::UI::percentToScreenHeight(0.75f);
        const float subsHoriSpacing = pe::UI::percentToScreenWidth(0.2f);

        _subscript.setPosition(
            _fieldText.getPosition().x + _fieldText.getGlobalBounds().width / 2.f + subsHoriSpacing, 
            (_fieldText.getPosition().y - _fieldText.getGlobalBounds().height / 2.f) + subsVertSpacing
        );
        surface.draw(_subscript);
    }

    connectToParent(surface);

    if (!_hideInterface && (!_isArmed || getBounds().contains(_mPos.x, _mPos.y)) && !isSelectingMovement()) {
        _plusButton.setPosition(_pos.x + getBounds().width - _plusButton.getSize().x, _pos.y + getBounds().height - _plusButton.getSize().y);
        _minusButton.setPosition(_pos.x, _pos.y + getBounds().height - _minusButton.getSize().y);

        if (_plusButton.getGlobalBounds().contains(_mPos.x, _mPos.y)) {
            _plusButton.setTextureRect({
                0, 16, 16, 16
            });
        } else {
            _plusButton.setTextureRect({
                0, 0, 16, 16
            });
        }

        if (_minusButton.getGlobalBounds().contains(_mPos.x, _mPos.y)) {
            _minusButton.setTextureRect({
                16, 16, 16, 16
            });
        } else {
            _minusButton.setTextureRect({
                16, 0, 16, 16
            });
        }

        surface.draw(_plusButton);
        if (hasParent()) surface.draw(_minusButton);
    }

    if (_hasMovement && _endPointNode != nullptr && _endPointNode->isActive()) {
        drawMovementLine(surface);
        if (_selectingMovement) _selectingMovement = false;
    } else if (_hasMovement && (_endPointNode == nullptr || !_endPointNode->isActive())) {
        _hasMovement = false;
        _endPointNode = nullptr;
    } else if (!_hasMovement && _selectingMovement) {
        drawMovementLine(surface);
    } else if (!_hasMovement) {
        _movementLineVertex = getPosition().y + getBounds().height;
    }
}

void VisualNode::drawMovementLine(sf::RenderTexture& surface) {
    const sf::Vector2f p0 = {
        getBounds().left + getBounds().width / 2.f,
        getBounds().top + getBounds().height + pe::UI::percentToScreenHeight(0.5f)
    };
    const sf::Vector2f p1 = {
        _hasMovement || _endPointNode != nullptr ? 
        _endPointNode->getBounds().left + _endPointNode->getBounds().width / 2.f 
        : _mPos.x,

        _hasMovement || _endPointNode != nullptr ? 
        _endPointNode->getBounds().top + _endPointNode->getBounds().height + pe::UI::percentToScreenHeight(0.5f) 
        : _mPos.y + pe::UI::percentToScreenHeight(4.f)
    };

    sf::Vector2f control = (0.5f + _curveAngle) * (p0 + p1);
    control.y += (-500.f - _curveHeight) + (p0.y + p1.y) / 2.f;

    std::vector<Line> lines;

    const int segments = 20;
    for (int i = 0; i < segments; ++i) {
        float t0 = i / float(segments);
        float t1 = (i + 1) / float(segments);

        const auto bez = [&](float t) {
            float u = 1.f - t;
            return u * u * p0 + 2 * u * t * control + t * t * p1;
            };

        sf::Vector2f a = bez(t0);
        sf::Vector2f b = bez(t1);

        lines.emplace_back(a, b, 4.f, sf::Color::Black);
    }

    for (const auto& line : lines) {
        _movementLineVertex = std::max(_movementLineVertex, std::max(line.point1.y, line.point2.y));
        surface.draw(line);
    }


    const auto& lastLine = lines.at(lines.size() - 1);
    const float angle = std::atan2(lastLine.point1.y - lastLine.point2.y, lastLine.point1.x - lastLine.point2.x) + pe::degToRads(270.f);

    constexpr float arrowSize = 20.f;
    constexpr float flair = 4.f;
    sf::ConvexShape arrow(4);
    sf::Vector2f arrowVertices[4] = {
        { (p1.x - arrowSize / 2.f), (p1.y + flair) },
        { p1.x, p1.y - arrowSize },
        { p1.x + arrowSize / 2.f, p1.y + flair },
        { p1.x, p1.y }
    };

    for (int i = 0; i < 4; i++) {
        const sf::Vector2f originalVertices = arrowVertices[i] - p1;
        arrowVertices[i].x = originalVertices.x * std::cos(angle) - originalVertices.y * std::sin(angle);
        arrowVertices[i].y = originalVertices.x * std::sin(angle) + originalVertices.y * std::cos(angle);
        arrow.setPoint(i, arrowVertices[i] + p1);
    }
    arrow.setFillColor(sf::Color::Black);

    surface.draw(arrow);
}

bool VisualNode::anotherNodeIsBlocking() const {
    for (const auto& node : VisualTree::getNodes()) {
        if (node->isActive() && node->getIdentifier() != getIdentifier() && node->hasMousePriority()) return true;
    }

    return false;
}

void VisualNode::mouseButtonPressed(const int mx, const int my, const int button) {
    if (anotherNodeIsBlocking()) return;

    _mouseDown = true;

    if (_plusButton.getGlobalBounds().contains(mx, my) || (hasParent() && _minusButton.getGlobalBounds().contains(mx, my))) {
        _clickingButtons = true;
    }
}

void VisualNode::mouseButtonReleased(const int mx, const int my, const int button) {
    if (_isArmed && !getBounds().contains(mx, my)) _isArmed = false;

    if (anotherNodeIsBlocking() && !isSelectingMovement()) return;

    _isArmed = !_clickingButtons && getBounds().contains(mx, my) && button == sf::Mouse::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
    _mouseDown = false;

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && !isSelectingMovement()) {
        if (_plusButton.getGlobalBounds().contains(mx, my) && getBounds().contains(_mPos.x, _mPos.y) && button == sf::Mouse::Left) {
            addChild();
        } else if (hasParent() && _minusButton.getGlobalBounds().contains(mx, my) && getBounds().contains(_mPos.x, _mPos.y) && button == sf::Mouse::Left) {
            hide();
        } else if (getBounds().contains(_mPos.x, _mPos.y) && button == sf::Mouse::Right) {
            const auto& menu = pe::UI::getMenu("subscriptMenu");
            menu->open();
            menu->getComponent("subscriptField")->getText().setString(_subscript.getString());
            _enteringSubscript = true;
        }
    } 
    
    if (getBounds().contains(_mPos.x, _mPos.y) && button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        if (!isSelectingMovement() && !_hasMovement) {
            bool otherNode = false;
            for (const auto& node : VisualTree::getNodes()) {
                if (node->isSelectingMovement()) {
                    otherNode = true;
                    break;
                }
            }

            if (!otherNode) {
                _selectingMovement = true;
                _curveHeight = 0.f;
                _curveAngle = 0.f;
            }
        } else if (_hasMovement) {
            _hasMovement = false;
            _endPointNode = nullptr;
        }
    } else if (isSelectingMovement() && button == sf::Mouse::Left) {
        for (const auto& node : VisualTree::getNodes()) {
            if (node->getIdentifier() != getIdentifier() && node->getBounds().contains(mx, my)) {
                _hasMovement = true;
                _endPointNode = node;
                break;
            }
        }

        if (!_hasMovement) _selectingMovement = false;
    }

    _clickingButtons = false;
}

void VisualNode::mouseMoved(const int mx, const int my) {
    if (anotherNodeIsBlocking()) return;

    if (isSelectingMovement()) {
        bool foundNode = false;
        for (const auto& node : VisualTree::getNodes()) {
            if (node->getIdentifier() != getIdentifier() && node->getBounds().contains(mx, my)) {
                _endPointNode = node;
                foundNode = true;
                break;
            }
        }

        if (!foundNode) _endPointNode = nullptr;
    }

    _mPos.x = mx;
    _mPos.y = my;
}

void VisualNode::mouseWheelScrolled(sf::Event::MouseWheelScrollEvent mouseWheelScroll) {
    if (isSelectingMovement()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            _curveHeight += 50.f * mouseWheelScroll.delta;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            _curveAngle += 0.05f * mouseWheelScroll.delta;
        }
    }
}

bool VisualNode::hasMousePriority() const {
    return getBounds().contains(_mPos.x, _mPos.y) || isSelectingMovement();
}

void VisualNode::releasePriority() {
    _mPos = { 0, 0 };
}

bool VisualNode::isSelectingMovement() const {
    return _selectingMovement;
}

bool VisualNode::hasMovement() const {
    return _hasMovement;
}

float VisualNode::getMovementLineVertex() const {
    return _movementLineVertex;
}

void VisualNode::textEntered(const sf::Uint32 character) {
    if (_isArmed) {
        sf::String userInput = _fieldText.getString();
        if (character == '\b' && userInput.getSize()) {
            userInput.erase(userInput.getSize() - 1, 1);
        } else if (character != '\b' && character != (char)22) {
            userInput += character;
        }
        _fieldText.setString(userInput);
    }
}

std::vector<s_p<VisualNode>>& VisualNode::getChildren() {
    return _children;
}

VisualNode* VisualNode::getParent() {
    return _parent;
}

bool VisualNode::hasChildren() {
    return getChildren().size() > 0;
}

bool VisualNode::hasParent() const {
    return _parent != nullptr;
}

bool VisualNode::isHovered() const {
    return !_hideInterface;
}

bool VisualNode::isArmed() const {
    return _isArmed;
}

bool VisualNode::hasSubscript() const {
    return _subscript.getString() != "" && _subscript.getString() != " ";
}
