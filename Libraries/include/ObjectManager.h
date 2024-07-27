#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <memory>

#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/norm.hpp>
#include <Box2D/box2d/box2d.h>
#include <Renderer.h>
#include <Camera.h>
#include <fstream>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class ObjectManager
{
public:
    void AddObject(b2Body* body, unsigned int texture, glm::vec2 scale, b2BodyType type, float rotation = 0.0f, glm::vec3 position = glm::vec3(0.0f), glm::vec4 color = glm::vec4(1.0f));
    void UpdateAllTransforms();
    void DrawImGui(b2World& world);
    void RenderAll(Camera& camera, Renderer& renderer);
    void MakeWorld();

    void SaveObjects(const std::string& filename) const;

    void LoadObjects(const std::string& filename, b2World& world);

    void SaveScenes() const;
    void LoadScenes();

    void LoadScene(std::string sceneName);

    std::shared_ptr<Object> FindObjectByName(std::string name);
    std::vector<std::string> FindAllObjectNames();

    void HandleMouseInteractions(GLFWwindow* window, Camera& camera);

    int FindObjectIndex(std::shared_ptr<Object> obj);

    std::shared_ptr<Object> InstantiateObject(std::shared_ptr<Object>& object, glm::vec3 position, float rotation, glm::vec2 scale);

    std::vector<std::filesystem::directory_entry> ListFilesInDirectory(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions);
    void DisplayDirectoryContents(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions);
    void ShowFileBrowser(bool* p_open);
    void SetAllResources();

    void PlayScene();
    void StopScene(b2World& world);

    void DrawObjectTree(std::shared_ptr<Object> obj, int depth);

    void SetWorld(b2World& world);

    std::vector<std::string> texturePaths;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Light> lights;
    std::vector<ShadowCaster> casters;
    Camera* globalCamera;
    b2World* globalWorld;

    std::vector<std::string> scenes;
    int selectedSceneIndex = -1;

    bool playMode = false;
private:
    int selectedObjectIndex = -1;
    int selectedVertexIndex = -1;
    bool isDraggingVertex = false;

    std::string newSceneName;
    bool showSceneNamePopup = false;
};

#endif