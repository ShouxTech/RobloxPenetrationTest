#pragma once
#include <Windows.h>

class PathParser {
    private:
        HANDLE Handle;
        DWORD Game;
    public:
        PathParser(HANDLE handle, DWORD game);

        DWORD ParsePath(std::string path);
};