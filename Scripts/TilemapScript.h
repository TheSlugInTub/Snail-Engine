#ifndef TEXT_SCRIPT
#define TEXT_SCRIPT

#include "Script.h"
#include <ScriptFactory.h>
#include <glm/glm.hpp>
#include <string>

class TilemapScript : public Script
{
public:
    TilemapScript();
    ~TilemapScript() override;

    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& j) override;
    std::string getTypeName() const override;

    void Start() override;
    void Update() override;
    void DrawImGui() override;

    std::vector<std::string> textures;
    int selectedTileIndex;
    std::string currentTexturePath;

    Tilemap tilemap;
};

#endif