// Copyright (c) 2025 Josh Sellers
// Licensed under the MIT License. See LICENSE file.

#ifndef _PERSISTENCE_H
#define _PERSISTENCE_H

#include <vector>
#include <string>
#include "../../PennyEngine/core/Defines.h"
#include "../visual/VisualNode.h"

class PersistenceImpl {
public:
    void save(std::string path);
    void load(std::string path);
private:
    const std::vector<std::string> _operators = { "!=", "==", ">=", "<=", "+", "*", "/", "=", ";", ",", ":", "(", ")", "{", "}", ">", "<", "!", "%" };
    std::vector<std::string> tokenize(std::string inScript);
    std::vector<std::string> splitOperators(std::string bareToken);

    void createNode(std::vector<std::string> lines);
    s_p<VisualNode> findNode(const std::string id);

    std::vector<std::pair<std::string, std::vector<std::string>>> _children;
    std::vector<std::pair<std::string, std::string>> _endPoints;
};

class Persistence {
public:
    static void save(std::string path) {
        _instance.save(path);
    }

    static void load(std::string path) {
        _instance.load(path);
    }

private:
    static inline PersistenceImpl _instance;
};

#endif