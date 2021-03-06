#include <iostream>
#include <vector>
#include "PathParser.h"
#include "StringUtility.h"
#include "Memory.h"

PathParser::PathParser(DWORD game) {
    this->Game = game;
}

DWORD PathParser::ParsePath(std::string path) {
    DWORD currentIndex = this->Game;

    std::vector<std::string> indexes;
    indexes = StringUtility::Split(path, '.');

    currentIndex = Memory::GetService(this->Game, indexes.front());
    indexes.erase(indexes.begin());

    for (std::string index : indexes) {
        currentIndex = Memory::FindFirstChild(currentIndex, index);
    }

    return currentIndex;
}