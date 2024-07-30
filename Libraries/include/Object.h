#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <Box2D/box2d/box2d.h>
#include <string>
#include <json.hpp>
#include <Script.h>
#include <CustomShape.h>
#include <Animation.h>
#include <Tilemap.h>

// An object, these can only be created via the object manager.
class Object : public std::enable_shared_from_this<Object>
{
public:
	Object(b2Body* objectBody, unsigned int objectTexture, glm::vec2 objectScale, b2BodyType objectBodyType, float objectRotation = 0.0f, glm::vec3 objectPosition = glm::vec3(0.0f), glm::vec4 objectColor = glm::vec4(1.0f));

	void UpdateTransform();

	nlohmann::json toJson() const;
	static std::shared_ptr<Object> fromJson(const nlohmann::json& j, b2World& world);

	void AddScript(std::unique_ptr<Script> script);
	void RemoveScript(const std::string& scriptName);

	void CreateCustomCollider(b2Body* body);

	void AddChild(std::shared_ptr<Object> child);
	void RemoveChild(std::shared_ptr<Object> child);

	void AddAnimation(const std::string& name, Animation animation);
	void SetCurrentAnimation(const std::string& name);
	void Animate();
	void StopAnimating();

	glm::vec3 position;
	glm::vec2 scale;
	glm::vec2 bodyScale;
	float rotation;
	unsigned int texture;
	glm::vec4 color;
	b2Body* body;
	bool isFlipped;
	std::string texturePath;
	b2BodyType bodyType; 
	std::vector<std::unique_ptr<Script>> scripts;
	std::string name;
	bool isRope;
	bool notLoadedCustomShape = false;
	bool isPlaying = false;
	bool bodyStatic = false;

	CustomShape shape;

	// Parenting and child objects aren't implemented yet, but these are here because I'll implement it in the future.
	std::shared_ptr<Object> parent = nullptr;
	std::vector<std::shared_ptr<Object>> children;

	std::map<std::string, Animation> animations;
	std::shared_ptr<Animation> currentAnimation = nullptr;

	// This really shouldn't be here, but I couldn't find another solution.
	Tilemap* tilemap = nullptr;

	bool operator==(const Object& other) const {
		if (other.position == position && other.color == color && other.body == body && other.name == name && other.scale == scale && other.texture == texture)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool operator==(const std::shared_ptr<Object>& other) const {
		if (other->position == position && other->color == color && other->body == body && other->name == name && other->scale == scale && other->texture == texture)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:

};

#endif