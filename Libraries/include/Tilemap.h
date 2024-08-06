#ifndef TILEMAP_H
#define TILEMAP_H

#include <glm/glm.hpp>
#include <vector>
#include <json.hpp>

struct Tile
{
	glm::vec2 position;
	unsigned int texture;
	std::string texturePath;

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

	nlohmann::json toJson() const 
	{
		nlohmann::json j = {
			{"position", {position.x, position.y}},
			{"texturePath", texturePath}
		};

		return j;
	}

	static Tile fromJson(const nlohmann::json& j) 
	{
		glm::vec2 position = { j["position"][0], j["position"][1] };
		std::string texturePath = j["texturePath"];

		Tile tile(position, 0, texturePath);

		return tile;
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

	nlohmann::json toJson() const;
	static Tilemap fromJson(const nlohmann::json& j);

	std::vector<Tile> tiles;
	glm::vec2 scale;
};

#endif