#include "Console.h"

std::vector<std::string> Console::messages;
bool Console::scrollToBottom = false;

void Console::Log(const std::string& message)
{
    messages.push_back(message);
    scrollToBottom = true; 
}

void Console::Draw()
{
    ImGui::Begin("Console");

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (const auto& message : messages)
        {
            ImGui::TextUnformatted(message.c_str());
        }

        if (scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom = false;
        }
    }
    ImGui::EndChild();

    ImGui::End();
}
