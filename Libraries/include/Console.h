#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include <string>
#include "imgui/imgui.h"

class Console
{
public:
    static void Log(const std::string& message);
    static void Draw();

private:
    static std::vector<std::string> messages;
    static bool scrollToBottom;
};

#endif 