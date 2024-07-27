#include <TilemapScript.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <SnailFunctions.h>
#include <Console.h>
#include <filesystem>

TilemapScript::TilemapScript()
{
    editorScript = true;
}

TilemapScript::~TilemapScript() = default;

nlohmann::json TilemapScript::toJson() const
{
    return
    {
        {"scripts", nlohmann::json::array()}
    };
}

void TilemapScript::fromJson(const nlohmann::json& j)
{

}

std::string TilemapScript::getTypeName() const
{
    return "TilemapScript";
}


void TilemapScript::Start() {
    
}

void TilemapScript::Update() {

    if (GetMouseButton(MouseKey::RightClick) && currentTexturePath != "")
    {
        auto obj = scriptObject.lock();
        obj->tilemap = &tilemap;

        auto manager = ScriptFactory::Instance().GetManager();

        double mouseX = GetMouseInputHorizontal();
        double mouseY = GetMouseInputVertical();

        glm::vec3 coords = manager->globalCamera->screenToWorldSpace(glm::vec2(mouseX, mouseY));

        // Round coords to the nearest integers
        glm::vec3 roundedCoords = glm::round(coords);

        unsigned int tex = loadTexture(currentTexturePath.c_str());

        Tile tile(glm::vec2(roundedCoords.x, roundedCoords.y), tex);

        tilemap.AddTile(tile);
    }
}


void TilemapScript::DrawImGui() {

    ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
    std::vector<std::string> names = objectManager->texturePaths;


    ImGui::Begin("TileManager");

    if (ImGui::BeginCombo("Tile texture path", currentTexturePath.c_str())) {
        for (const auto& path : names) {
            bool isSelected = (currentTexturePath == path);
            if (ImGui::Selectable(path.c_str(), isSelected)) {
                currentTexturePath = path;
                textures.push_back(currentTexturePath);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Text("Tiles");
    ImGui::Text(" ");

    for (size_t i = 0; i < textures.size(); ++i) {
        auto& object = textures[i];
        ImGui::PushID(static_cast<int>(i));

        std::filesystem::path fullPath(object);
        std::string filename = fullPath.filename().string();

        bool selected = selectedTileIndex == static_cast<int>(i);
        if (ImGui::Selectable(filename.c_str(), selected)) {
            selectedTileIndex = static_cast<int>(i);
            currentTexturePath = object;
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(size_t));
            ImGui::Text("Dragging texture %d", i);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                IM_ASSERT(payload->DataSize == sizeof(size_t));
                size_t payload_n = *(const size_t*)payload->Data;
                if (payload_n != i) {
                    std::swap(textures[payload_n], textures[i]);
                    if (selectedTileIndex == static_cast<int>(i))
                        selectedTileIndex = static_cast<int>(payload_n);
                    else if (selectedTileIndex == static_cast<int>(payload_n))
                        selectedTileIndex = static_cast<int>(i);
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
    }

    ImGui::End();
}

REGISTER_SCRIPT(TilemapScript)
