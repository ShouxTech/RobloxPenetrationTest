#include <iostream>
#include <vector>
#include <sstream>
#include "PathParser.h"
#include "Memory.h"

PathParser::PathParser(HANDLE handle, DWORD game) {
    this->Handle = handle;
    this->Game = game;
}

std::vector<std::string> PathParser::Split(std::string str, char delimiter) {
    std::vector<std::string> split;

    std::stringstream stringStream(str);
    std::string splitString;
    while (std::getline(stringStream, splitString, delimiter)) {
        split.push_back(splitString);
    }

    return split;
}

DWORD PathParser::ParsePath(std::string path) {
    DWORD currentIndex = this->Game;

    std::vector<std::string> indexes;
    indexes = this->Split(path, '.');

    currentIndex = Memory::GetService(this->Handle, this->Game, indexes.front());
    indexes.erase(indexes.begin());

    for (std::string index : indexes) {
        currentIndex = Memory::FindFirstChild(this->Handle, currentIndex, index);
    }

    return currentIndex;
}