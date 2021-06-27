#pragma once
#include <Windows.h>

class PathParser {
    private:
        HANDLE Handle;
        DWORD Game;
    public:
        PathParser(HANDLE handle, DWORD game);

        std::vector<std::string> Split(std::string str, char delimiter);

        DWORD ParsePath(std::string path);
};