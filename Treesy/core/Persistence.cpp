// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#include "Persistence.h";
#include "../../PennyEngine/core/Util.h"
#include "../../PennyEngine/core/Logger.h"
#include "../visual/VisualTree.h"
#include <fstream>
#include <iostream>

static void write(std::ofstream& out, std::string output) {
    out << output << std::endl;
}

void PersistenceImpl::save(std::string path) {
    std::ofstream out(path);

    const auto& nodes = VisualTree::getNodes();
    for (const auto& node : nodes) {
        write(out, "{");
        write(out, "id: " + node->getIdentifier());
        write(out, "pos: " + std::to_string(node->getPosition().x) + ", " + std::to_string(node->getPosition().y));
        if (node->hasParent()) write(out, "parent: " + node->getParent()->getIdentifier());
        if (node->hasChildren()) {
            out << "children: ";
            for (int i = 0; i < node->getChildren().size(); i++) {
                const auto& child = node->getChildren().at(i);
                out << child->getIdentifier() + std::string(i < node->getChildren().size() - 1 ? ", " : "");
            }
            out << std::endl;
        }
        write(out, "text: \"" + node->getText().getString() + "\"");
        write(out, "subs: \"" + node->getSubscript() + "\"");
        write(out, "hasMovement: " + std::string(node->hasMovement() ? "true" : "false"));
        if (node->hasMovement()) write(out, "endPointNode: " + node->_endPointNode->getIdentifier());
        write(out, "curveAngle: " + std::to_string(node->_curveAngle));
        write(out, "curveHeight: " + std::to_string(node->_curveHeight));
        write(out, "}");
    }
}

void PersistenceImpl::load(std::string path) {
    std::ifstream in(path);
    if (in.good()) {
        try {
            std::string line;
            bool readingNode = false;
            std::vector<std::string> nodeLines;
            while (getline(in, line)) {
                if (!readingNode && line == "{") readingNode = true;
                else if (readingNode && line == "}") {
                    readingNode = false;
                    createNode(nodeLines);
                    nodeLines.clear();
                } else if (readingNode) nodeLines.push_back(line);
            }
        } catch (std::exception ex) {
            pe::Logger::log(ex.what());
        }
    }

    for (const auto& pair : _children) {
        const auto& parent = findNode(pair.first);
        if (parent == nullptr) {
            pe::Logger::log("Did not find parent " + pair.first);
            continue;
        }

        for (const auto& childId : pair.second) {
            const auto& child = findNode(childId);
            if (child == nullptr) {
                pe::Logger::log("Did not find child " + childId + " of parent " + pair.first);
                continue;
            }

            parent->_children.push_back(child);
        }
    }

    for (const auto& pair : _endPoints) {
        const auto& startNode = findNode(pair.first);
        const auto& endNode = findNode(pair.second);

        if (startNode == nullptr) {
            pe::Logger::log("Did not find startNode " + pair.first);
            continue;
        }

        if (endNode == nullptr) {
            pe::Logger::log("Did not find endNode + " + pair.second + " for startNode " + pair.first);
            continue;
        }

        if (startNode->hasMovement()) {
            startNode->_endPointNode = endNode;
        } else pe::Logger::log("Found endNode for node that didn't have movement: " + pair.first + ", " + pair.second);
    }

    _children.clear();
    _endPoints.clear();
}

void PersistenceImpl::createNode(std::vector<std::string> lines) {
    std::string id = "";
    sf::Vector2f pos;
    std::string parent = "";
    std::vector<std::string> children;
    std::string fieldText = "";
    std::string subscript = "";
    bool hasMovement = false;
    std::string endPointNode = "";
    float curveAngle = 0.f;
    float curveHeight = 0.f;

    for (const auto& line : lines) {
        const std::vector tokens = tokenize(line);
        if (tokens.at(1) == ":") {
            const std::string var = tokens.at(0);
            if (var == "id") id = tokens.at(2);
            else if (var == "pos") {
                pos = { std::stof(tokens.at(2)), std::stof(tokens.at(4)) };
            } else if (var == "parent") parent = tokens.at(2);
            else if (var == "children") {
                for (int i = 2; i < tokens.size(); i += 2) {
                    children.push_back(tokens.at(i));
                }
            } else if (var == "text") fieldText = tokens.at(2);
            else if (var == "subs") subscript = tokens.at(2);
            else if (var == "hasMovement") hasMovement = tokens.at(2) == "1";
            else if (var == "endPointNode") endPointNode = tokens.at(2);
            else if (var == "curveAngle") curveAngle = std::stof(tokens.at(2));
            else if (var == "curveHeight") curveHeight = std::stof(tokens.at(2));
        }
    }

    pe::replaceAll(fieldText, "\"", "");
    pe::replaceAll(subscript, "\"", "");

    const auto& res = PennyEngine::getRenderResolution();
    pos.x = pos.x / res.width * 100.f;
    pos.y = pos.y / res.height * 100.f;

    s_p<VisualNode> parentNode = nullptr;
    if (parent != "") parentNode = findNode(parent);
    const s_p<VisualNode> node = new_s_p(VisualNode, (parentNode.get(), pos.x, pos.y, id));

    node->getText().setString(fieldText);
    node->_subscript.setString(subscript);
    node->_hasMovement = hasMovement;
    node->_curveAngle = curveAngle;
    node->_curveHeight = curveHeight;
    VisualTree::_instance._nodes.push_back(node);
    VisualTree::_instance._renderNodes.push_back(node);

    if (children.size() != 0) {
        _children.push_back({ id, children });
    }

    if (hasMovement) {
        _endPoints.push_back({ id, endPointNode });
    }
}

s_p<VisualNode> PersistenceImpl::findNode(const std::string id) {
    for (const auto& node : VisualTree::_instance._nodes) {
        if (node->getIdentifier() == id) return node;
    }

    return nullptr;
}

std::vector<std::string> PersistenceImpl::tokenize(std::string inScript) {
    std::string script = "";
    bool replaceSpaces = false;
    for (auto symbol : inScript) {
        if (replaceSpaces && symbol == ' ') {
            script += "RPLSPC";
        } else if (replaceSpaces && symbol != '"') {
            for (int i = 4; i < _operators.size(); i++) {
                if (symbol == _operators.at(i).at(0)) {
                    script += "RPL" + std::to_string(i);
                    break;
                }
            }
        }

        if (symbol != ' ' || !replaceSpaces) {
            bool isOperator = false;
            for (int i = 4; i < _operators.size(); i++) {
                if (symbol == _operators.at(i).at(0)) {
                    isOperator = true;
                    break;
                }
            }
            if (!isOperator || !replaceSpaces) {
                script += std::string(1, symbol);
            }
        }
        if (symbol == '"') replaceSpaces = !replaceSpaces;
    }

    std::vector<std::string> tokens;

    std::vector<std::string> bareTokens = pe::splitString(script, " ");
    for (std::string& bareToken : bareTokens) {
        pe::replaceAll(bareToken, "RPLSPC", " ");
    }

    for (std::string bareToken : bareTokens) {
        std::vector<std::string> operatorTokens = splitOperators(bareToken);
        for (std::string token : operatorTokens) {
            if (token != "")
                tokens.push_back(token);
        }
    }

    for (std::string& token : tokens) {
        for (int i = 4; i < _operators.size(); i++) {
            pe::replaceAll(token, "RPL" + std::to_string(i), _operators.at(i));
        }

        if (!pe::stringStartsWith(token, "\"")) {
            pe::replaceAll(token, "true", "1");
            pe::replaceAll(token, "false", "0");
        }
    }

    return tokens;
}

std::vector<std::string> PersistenceImpl::splitOperators(std::string bareToken) {
    std::vector<std::string> operatorExpressionTokens;
    for (std::string operatorString : _operators) {
        if (bareToken.find(operatorString) != std::string::npos) {
            std::vector<std::string> tokens = pe::splitString(bareToken, operatorString);
            std::vector<std::string> previousTokens = splitOperators(tokens.at(0));
            for (std::string previousToken : previousTokens) {
                operatorExpressionTokens.push_back(previousToken);
            }

            std::vector<std::string> additionalExpressions;
            for (int i = 1; i < tokens.size(); i++) {
                additionalExpressions.push_back(operatorString);
                for (std::string subToken : splitOperators(tokens.at(i))) {
                    additionalExpressions.push_back(subToken);
                }
            }

            for (std::string expression : additionalExpressions) {
                operatorExpressionTokens.push_back(expression);
            }

            return operatorExpressionTokens;
        }
    }

    operatorExpressionTokens.push_back(bareToken);
    return operatorExpressionTokens;
}