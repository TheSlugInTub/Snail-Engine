#include <Tilemap.h>

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