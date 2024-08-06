#include <Tilemap.h>
#include <SnailFunctions.h>

Tilemap::Tilemap(glm::vec2 tileScale)
{
	scale = tileScale;
}

Tilemap::Tilemap()
{
    scale = glm::vec2(1.0f, 1.0f);
}

void Tilemap::AddTile(Tile& tile)
{
	tiles.push_back(tile);
}

void Tilemap::RemoveTile(Tile& tile)
{
    auto it = std::find(tiles.begin(), tiles.end(), tile);
    int index = std::distance(tiles.begin(), it);
    tiles.erase(it);
}

int Tilemap::GetTileIndex(Tile& tile)
{
    auto it = std::find(tiles.begin(), tiles.end(), tile);
    return std::distance(tiles.begin(), it);
}

nlohmann::json Tilemap::toJson() const
{
	nlohmann::json j;
	j["scale"] = { scale.x, scale.y };
	j["tiles"] = nlohmann::json::array();

	for (const auto& tile : tiles)
	{
		j["tiles"].push_back(tile.toJson());
	}

	return j;
}

Tilemap Tilemap::fromJson(const nlohmann::json& j)
{
	Tilemap tilemap;
	tilemap.scale = glm::vec2(j["scale"][0], j["scale"][1]);

	for (const auto& tileJson : j["tiles"])
	{
		tilemap.tiles.push_back(Tile::fromJson(tileJson));
	}

	for (auto& tile : tilemap.tiles)
	{
		tile.texture = loadTexture(tile.texturePath.c_str());
	}

	return tilemap;
}