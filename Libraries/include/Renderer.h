#ifndef RENDERER_H
#define RENDERER_H

#include <Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <Box2D/box2d/box2d.h>
#include <vector>
#include <Camera.h>
#include <SnailFunctions.h>
#include <Object.h>
#include <Window.h>

struct Light
{
	glm::vec3 position;
	float innerRadius;
	float outerRadius;
	glm::vec4 color;
	float intensity;

	bool operator==(const Light& other) const {
		if (other.position == position && other.innerRadius == innerRadius && other.outerRadius == outerRadius && other.color == color && other.intensity == intensity)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct ShadowCaster
{
	glm::vec3 position;
	glm::vec2 scale;
	float rotation;
	unsigned int texture;
	bool isFlipped;

	bool operator==(const ShadowCaster& other) const {
		if (other.position == position && other.scale == scale && other.rotation == rotation && other.texture == texture)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Renderer
{
public:
	Renderer();

	void RenderObject(Object& object, const glm::mat4& view, const glm::mat4& projection, std::vector<Light>& lights, std::vector<ShadowCaster>& casters);
	void RenderBodyVertices(Shader& shader, Camera& camera, Object& object, const glm::mat4& view, const glm::mat4& projection);
	void RenderCustomShape(Shader& shader, Camera& camera, Object& object, const glm::mat4& view, const glm::mat4& projection);
	void RenderRope(std::vector<glm::vec2> points, const glm::mat4& view, const glm::mat4& projection);
	void Render3dPoints(const std::vector<glm::vec3>& points, const glm::mat4& view, const glm::mat4& projection);
	void RenderTilemap(Tilemap& tilemap, const glm::mat4& view, const glm::mat4& projection);

	Shader defaultShader;
	Shader lineShader;
private:
	unsigned int VAO, VBO, EBO;
};

#endif