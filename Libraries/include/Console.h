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

    // Used for when there are too many messages to display and it scrolls to the end.
    static bool scrollToBottom;
};

#endif 