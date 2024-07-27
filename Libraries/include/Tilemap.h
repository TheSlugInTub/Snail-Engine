#ifndef TILEMAP_H
#define TILEMAP_H

#include <glm/glm.hpp>
#include <vector>

struct Tile
{
	glm::vec2 position;
	unsigned int texture;

	bool operator==(const Tile& other) const
	{
		if (other.position == position)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Tilemap
{
public:
	Tilemap(glm::vec2 tileScale);
	Tilemap();

	void AddTile(Tile& tile);
	void RemoveTile(Tile& tile);
	int GetTileIndex(Tile& tile);

	std::vector<Tile> tiles;
	glm::vec2 scale;
};

#endif