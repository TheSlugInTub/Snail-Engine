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

// The object manager, handles all the objects, and object creation.
class ObjectManager
{
public:
    void AddObject(b2Body* body, unsigned int texture, glm::vec2 scale, b2BodyType type, float rotation = 0.0f, glm::vec3 position = glm::vec3(0.0f), glm::vec4 color = glm::vec4(1.0f));
    void RemoveObject(const std::shared_ptr<Object> obj);

    void UpdateAllTransforms();
    
    void DrawImGui(b2World& world);
    
    // Render all the objects in the scene.
    void RenderAll(Camera& camera, Renderer& renderer);

    void SaveObjects(const std::string& filename) const;
    void LoadObjects(const std::string& filename, b2World& world);

    void SaveScenes() const;
    void LoadScenes();

    // Function only to be used by scripts to load a new scene.
    void LoadScene(std::string sceneName);

    std::shared_ptr<Object> FindObjectByName(std::string name);
    std::vector<std::string> FindAllObjectNames();

    // Pretty much only for dragging around vertices for a custom collider.
    void HandleMouseInteractions(GLFWwindow* window, Camera& camera);

    int FindObjectIndex(std::shared_ptr<Object> obj);

    std::shared_ptr<Object> InstantiateObject(std::shared_ptr<Object>& object, glm::vec3 position, float rotation, glm::vec2 scale);

    std::vector<std::filesystem::directory_entry> ListFilesInDirectory(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions);
    void DisplayDirectoryContents(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions);
    void ShowFileBrowser(bool* p_open);

    // This is for updating the resources when a new file is added to the resources folder.
    void SetAllResources();

    void PlayScene();
    void StopScene(b2World& world);

    // This is for loading and saving all the different camera saves to a Camera.json file.
    void SaveCamera() const;
    void LoadCamera();

    void SetWorld(b2World& world);

    std::vector<std::string> texturePaths;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Light> lights;
    std::vector<ShadowCaster> casters;
    Camera* globalCamera;
    b2World* globalWorld;

    std::vector<std::string> scenes;

    // This holds all the different camera values for different scenes.
    std::vector<CameraSave> cameraSaves;

    // Scene you're currently in.
    int selectedSceneIndex = -1;

    bool playMode = false;
private:
    // Selected object for the editor, so it knows which object's properties to display.
    int selectedObjectIndex = -1;

    // These are for dragging the vertices of a custom collider.
    int selectedVertexIndex = -1;
    bool isDraggingVertex = false;

    // Name buffer for creating a new scene
    std::string newSceneName;

    // Pop up for creating a new scene
    bool showSceneNamePopup = false;
};

#endif