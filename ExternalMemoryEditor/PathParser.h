#pragma once
#include <Windows.h>

class PathParser {
    private:
        DWORD Game;
    public:
        PathParser(DWORD game);

        DWORD ParsePath(std::string path);
};