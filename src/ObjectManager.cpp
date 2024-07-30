#include "ObjectManager.h"
#include <imgui/imgui.h>
#include <iostream>
#include <ScriptFactory.h>
#include <SnailFunctions.h>
#include <Window.h>

void ObjectManager::SetAllResources()
{
    std::string directoryPath = "Resources/";

    if (texturePaths.size() != 0)
    {
        texturePaths.clear();
    }

    try {
        // Check if the provided path is a directory
        if (std::filesystem::is_directory(directoryPath)) {
            // Iterate over the directory entries
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
                // Check if the entry is a regular file and has a .png extension
                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".png") {
                    texturePaths.push_back(entry.path().string());
                }
            }
        }
        else {
            std::cerr << "The provided path is not a directory: " << directoryPath << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "General exception: " << e.what() << std::endl;
    }
}

void ObjectManager::AddObject(b2Body* body, unsigned int texture, glm::vec2 scale, b2BodyType type, float rotation, glm::vec3 position, glm::vec4 color)
{
    std::shared_ptr<Object> object = std::make_shared<Object>(body, texture, scale, type, rotation, position, color);
    objects.push_back(object);
}

void ObjectManager::RemoveObject(const std::shared_ptr<Object> obj)
{
    auto it = std::find(objects.begin(), objects.end(), obj);

    // If found, erase it
    if (it != objects.end()) {
        objects.erase(it);
        if (obj->body) {
            globalWorld->DestroyBody(obj->body); 
        }
    }
    selectedObjectIndex = -1; 
}

void ObjectManager::UpdateAllTransforms()
{
    for (auto& object : objects)
    { 
        object->UpdateTransform();
    }
}

void ObjectManager::SetWorld(b2World& world)
{
    globalWorld = &world;
}

void ObjectManager::DrawImGui(b2World& world) {
    ImGui::Begin("Objects");
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Add Static Physics Object")) {
            b2BodyDef bodyDef2;
            bodyDef2.type = b2_staticBody;
            bodyDef2.position.Set(0.0f, 0.0f);
            b2Body* body2 = world.CreateBody(&bodyDef2);

            b2PolygonShape staticBox;
            staticBox.SetAsBox(0.5f, 0.5f);

            b2FixtureDef fixtureDef2;
            fixtureDef2.shape = &staticBox;
            fixtureDef2.filter.categoryBits = 0x0001;
            fixtureDef2.filter.maskBits = 0xFFFF; // Collide with everything

            body2->CreateFixture(&fixtureDef2);

            unsigned int texture = loadTexture("Resources/Textures/DefaultTexture.png");

            AddObject(body2, texture, glm::vec2(1.0f), b2_staticBody, 0.0f, glm::vec3(0.0f), glm::vec4(1.0f));
        }
        if (ImGui::MenuItem("Add Physics Object")) {
            b2BodyDef bodyDef1;
            bodyDef1.type = b2_dynamicBody;
            bodyDef1.position.Set(0.0f, 0.0f);
            b2Body* body1 = world.CreateBody(&bodyDef1);

            b2PolygonShape dynamicBox1;
            dynamicBox1.SetAsBox(0.5f, 0.5f);

            b2FixtureDef fixtureDef1;
            fixtureDef1.shape = &dynamicBox1;
            fixtureDef1.density = 1.0f;
            fixtureDef1.friction = 0.8f;

            fixtureDef1.filter.categoryBits = 0x0001;
            fixtureDef1.filter.maskBits = 0xFFFF; // Collide with everything

            body1->CreateFixture(&fixtureDef1);

            unsigned int texture = loadTexture("Resources/Textures/DefaultTexture.png");

            AddObject(body1, texture, glm::vec2(1.0f), b2_dynamicBody, 0.0f, glm::vec3(0.0f), glm::vec4(1.0f));
        }

        if (ImGui::MenuItem("Add Object")) {
            b2BodyDef bodyDef2;
            bodyDef2.type = b2_staticBody;
            bodyDef2.position.Set(0.0f, 0.0f);
            b2Body* body2 = world.CreateBody(&bodyDef2);

            b2PolygonShape staticBox;
            staticBox.SetAsBox(0.5f, 0.5f);

            b2FixtureDef fixtureDef2;
            fixtureDef2.shape = &staticBox;
            fixtureDef2.filter.categoryBits = 0x0000;
            fixtureDef2.filter.maskBits = 0x0000; // Collide with everything

            body2->CreateFixture(&fixtureDef2);

            unsigned int texture = loadTexture("Resources/Textures/DefaultTexture.png");

            auto object = std::make_shared<Object>(body2, texture, glm::vec2(1.0f), b2_staticBody, 0.0f, glm::vec3(0.0f), glm::vec4(1.0f));
            object->bodyStatic = true;
            objects.push_back(object);
        }

        if (ImGui::MenuItem("Add Custom Collider Object")) {
            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position.Set(0.0f, 0.0f);
            b2Body* body = world.CreateBody(&bodyDef);

            CustomShape customShape;
            customShape.AddVertex(glm::vec2(-0.5f, -0.5f));
            customShape.AddVertex(glm::vec2(0.5f, -0.5f));
            customShape.AddVertex(glm::vec2(0.5f, 0.5f));
            customShape.AddVertex(glm::vec2(-0.5f, 0.5f));

            unsigned int texture = loadTexture("Resources/Textures/DefaultTexture.png");

            auto object = std::make_shared<Object>(body, texture, glm::vec2(1.0f), b2_dynamicBody, 0.0f, glm::vec3(0.0f), glm::vec4(1.0f));
            object->shape = customShape;
            object->CreateCustomCollider(body);

            objects.push_back(object);
        }
        
        ImGui::EndPopup();
    }

    for (size_t i = 0; i < objects.size(); ++i) {
        auto& object = objects[i];
        ImGui::PushID(static_cast<int>(i));

        bool selected = selectedObjectIndex == static_cast<int>(i);
        if (ImGui::Selectable(objects[i]->name.c_str(), selected)) {
            selectedObjectIndex = static_cast<int>(i);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(size_t));
            ImGui::Text("Dragging Object %d", i);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                IM_ASSERT(payload->DataSize == sizeof(size_t));
                size_t payload_n = *(const size_t*)payload->Data;
                if (payload_n != i) {
                    std::swap(objects[payload_n], objects[i]);
                    if (selectedObjectIndex == static_cast<int>(i))
                        selectedObjectIndex = static_cast<int>(payload_n);
                    else if (selectedObjectIndex == static_cast<int>(payload_n))
                        selectedObjectIndex = static_cast<int>(i);
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
    }
    ImGui::End();

    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objects.size()) {
        ImGui::Begin("Properties");

        auto& obj = objects[selectedObjectIndex];

        char nameBuffer[128];
        std::strncpy(nameBuffer, obj->name.c_str(), sizeof(nameBuffer));
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            obj->name = std::string(nameBuffer);
        }

        bool positionChanged = ImGui::DragFloat3("Position", glm::value_ptr(obj->position), 0.1f);
        ImGui::DragFloat2("Scale", glm::value_ptr(obj->scale), 0.1f);

        bool bodyScaleChanged = false;

        if (obj->bodyStatic == false) { bodyScaleChanged = ImGui::DragFloat2("Body Scale", glm::value_ptr(obj->bodyScale), 0.1f); }

        if (bodyScaleChanged) {
            b2Fixture* fixture = obj->body->GetFixtureList();
            while (fixture != nullptr) {
                b2Fixture* next = fixture->GetNext();
                obj->body->DestroyFixture(fixture);
                fixture = next;
            }

            b2PolygonShape boxShape;
            boxShape.SetAsBox(obj->bodyScale.x, obj->bodyScale.y); 

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &boxShape;

            if (obj->bodyType == b2_staticBody) {
                fixtureDef.density = 0.0f; // Static objects don't need density
                fixtureDef.friction = 0.3f; 
            }
            else if (obj->bodyType == b2_dynamicBody) {
                fixtureDef.density = 1.0f; 
                fixtureDef.friction = 0.3f; 
            }

            obj->body->CreateFixture(&fixtureDef);
        }

        bool rotationChanged = ImGui::DragFloat("Rotation", &obj->rotation, 0.1f);
        ImGui::ColorEdit4("Color", glm::value_ptr(obj->color));

        if (ImGui::BeginCombo("Texture Path", std::filesystem::path(obj->texturePath).filename().string().c_str())) {
            for (const auto& path : texturePaths) {
                bool isSelected = (obj->texturePath == path);
                if (ImGui::Selectable(std::filesystem::path(path).filename().string().c_str(), isSelected)) {
                    obj->texturePath = path;
                    obj->texture = loadTexture(obj->texturePath.c_str());
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Checkbox("Is flipped?", &obj->isFlipped);

        if (obj->shape.vertices.size() > 0) {
            if (ImGui::CollapsingHeader("Collider"))
            {
                if (ImGui::Button("Add Point")) {
                    obj->shape.AddVertex(glm::vec2(0.0f, 0.0f));
                    obj->CreateCustomCollider(obj->body);
                }

                HandleMouseInteractions(getWindow(), *globalCamera);

                for (size_t i = 0; i < obj->shape.vertices.size(); ++i) {
                    ImGui::PushID(static_cast<int>(i));
                    bool draggingVertex = ImGui::DragFloat2("Vertex", glm::value_ptr(obj->shape.vertices[i]), 0.1f);
                    if (draggingVertex)
                    {
                        obj->CreateCustomCollider(obj->body);
                    }
                    ImGui::PopID();
                }
            }
        }

        if (GetKeyDown(Key::Delete)) {
            world.DestroyBody(obj->body);
            objects.erase(objects.begin() + selectedObjectIndex);
            selectedObjectIndex = -1; // Deselect the object
        }

        for (auto& script : obj->scripts) {
            if (ImGui::CollapsingHeader(script->getTypeName().c_str())) {
                script->DrawImGui();
                if (ImGui::Button("Remove Script")) {
                    obj->RemoveScript(script->getTypeName());
                }
            }
        }

        if (positionChanged || rotationChanged) {
            obj->body->SetTransform(b2Vec2(obj->position.x, obj->position.y), obj->rotation);
            obj->position.z = obj->position.z;
            b2Vec2 force(0.00001f, 0.0f);
            obj->body->ApplyForceToCenter(force, true);
        }

        if (ImGui::BeginPopupContextWindow()) {
            auto scriptNames = ScriptFactory::Instance().GetRegisteredScriptNames();

            for (const auto& name : scriptNames) {
                if (ImGui::MenuItem(name.c_str())) {
                    auto script = ScriptFactory::Instance().CreateScript(name);
                    if (script) {
                        script->SetObject(obj);
                        obj->AddScript(std::move(script));
                    }
                }
            }

            ImGui::EndPopup();
        }

        ImGui::End();
    }

    ImGui::Begin("Scenes");

    for (size_t i = 0; i < scenes.size(); ++i) {
        auto& scene = scenes[i];
        ImGui::PushID(static_cast<int>(i));

        // Remove the .json extension
        std::string sceneName = scene;
        size_t extensionPos = sceneName.find_last_of('.');
        if (extensionPos != std::string::npos) {
            sceneName = sceneName.substr(0, extensionPos);
        }

        bool selected = selectedSceneIndex == static_cast<int>(i);
        if (ImGui::Selectable(sceneName.c_str(), selected)) {
            ScriptFactory::Instance().GetCanvas()->Clear();
            SaveObjects(scenes[selectedSceneIndex]);
            selectedSceneIndex = static_cast<int>(i);
            globalCamera->Save = cameraSaves[selectedSceneIndex];
            LoadObjects(scenes[selectedSceneIndex], *globalWorld);
            globalCamera->SaveFrustumCorners();
        }

        ImGui::PopID();
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Add new scene"))
        {
            showSceneNamePopup = true;
        }

        ImGui::EndPopup();
    }

    // Show the scene name input pop-up
    if (showSceneNamePopup)
    {
        ImGui::OpenPopup("New Scene");

        if (ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter the name of the new scene:");

            // Create a buffer for the scene name input
            static char buffer[128] = "";
            ImGui::InputText("Scene Name", buffer, IM_ARRAYSIZE(buffer));

            if (ImGui::Button("OK"))
            {
                newSceneName = buffer;
                if (!newSceneName.empty())
                {
                    std::filesystem::path scenePath = std::filesystem::path("Data") / (newSceneName + ".json");
                    std::filesystem::path scenePath2 = newSceneName + ".json";
                    // Add the new scene with the specified name
                    scenes.push_back(scenePath2.string());
                    std::ofstream file(scenePath);
                    file << "null";
                    file.close();
                    cameraSaves.push_back(globalCamera->Save);
                    SaveCamera();
                }

                // Close the pop-up
                ImGui::CloseCurrentPopup();
                showSceneNamePopup = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                showSceneNamePopup = false;
            }

            ImGui::EndPopup();
        }
    }

    ImGui::End();

    bool fileBrowserOpen = true;
    ShowFileBrowser(&fileBrowserOpen);
}

void ObjectManager::RenderAll(Camera& camera, Renderer& renderer)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), getAspectRatio(), 0.1f, 100.0f);

    globalCamera = &camera;

    for (auto& object : objects)
    {
        renderer.RenderObject(*object, view, projection, lights, casters);

        if (object->tilemap != nullptr)
        {
            renderer.RenderTilemap(*object->tilemap, view, projection);
        }

        if (object->bodyStatic == false && object->shape.vertices.size() == 0 && !playMode)
        {
            renderer.RenderBodyVertices(renderer.lineShader, camera, *object, view, projection);
        }

        if (object->isRope)
        {
            for (auto& script : object->scripts)
            {
                if (script->getTypeName() == "RopeScript")
                {
                    script->Update();
                }
            }
        }
        if (object->shape.vertices.size() > 0)
        {
            renderer.RenderCustomShape(renderer.lineShader, camera, *object, view, projection);
            if (!object->notLoadedCustomShape)
            {
                object->CreateCustomCollider(object->body);
                object->notLoadedCustomShape = true;
            }
        }
    }

    if (!playMode)
    {
        renderer.RenderRope(globalCamera->corners, view, projection);
    }
}

void ObjectManager::SaveObjects(const std::string& filename) const {

    std::filesystem::path directory = "Data";
    // Ensure the directory exists
    std::filesystem::create_directories(directory);

    std::filesystem::path filePath = directory / filename;

    nlohmann::json j;
    for (const auto& obj : objects) {
        j.push_back(obj->toJson());
    }

    std::ofstream file(filePath);
    file << j.dump(4);

    SaveCamera();
    SaveScenes();
}

void ObjectManager::LoadObjects(const std::string& filename, b2World& world) {
    
    std::filesystem::path directory = "Data";
    std::filesystem::path filePath = directory / filename;

    for (b2Body* body = world.GetBodyList(); body; ) {
        b2Body* nextBody = body->GetNext();
        world.DestroyBody(body);
        body = nextBody;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    nlohmann::json j;
    file >> j;

    objects.clear();

    for (const auto& objJson : j) {
        auto obj = Object::fromJson(objJson, world);
        if (obj) {
            objects.push_back(std::move(obj));
        }
        else {
            std::cerr << "Failed to create object from JSON" << std::endl;
        }
    }

    LoadCamera();
}

void ObjectManager::SaveScenes() const
{
    std::filesystem::path directory = "Data";
    std::filesystem::create_directories(directory);

    std::filesystem::path filePath = directory / "Scenes.json";

    nlohmann::json j;

    j = scenes;

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
    else {
        std::cerr << "Could not open file for writing: " << filePath << std::endl;
    }
}

void ObjectManager::LoadScenes() {
    std::filesystem::path directory = "Data";
    std::filesystem::path filePath = directory / "Scenes.json";

    std::vector<std::string> vec;
    nlohmann::json j;

    std::ifstream file(filePath);

    if (file.is_open()) {
        file >> j;  
        vec = j.get<std::vector<std::string>>();  
        file.close();
    }
    else {
        std::cerr << "Could not open file for reading: " << filePath << std::endl;
    }

    scenes.clear();
    scenes = vec;
}

void ObjectManager::LoadScene(std::string sceneName)
{
    auto camera = ScriptFactory::Instance().GetManager()->globalCamera;

    int index = -1;
    auto it = std::find(scenes.begin(), scenes.end(), sceneName);
    if (it != scenes.end()) {
        index = std::distance(scenes.begin(), it);
    }

    ScriptFactory::Instance().GetCanvas()->Clear();

    StopScene(*globalWorld);

    selectedSceneIndex = static_cast<int>(index);
    LoadObjects(scenes[index], *globalWorld);

    PlayScene();
}

std::shared_ptr<Object> ObjectManager::FindObjectByName(std::string name)
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->name == name)
        {
            return objects[i];
        }
    }

    return nullptr;
}

std::vector<std::string> ObjectManager::FindAllObjectNames()
{
    std::vector<std::string> names;

    for (auto Object : objects)
    {
        names.emplace_back(Object->name);
    }

    return names;
}

std::shared_ptr<Object> ObjectManager::InstantiateObject(std::shared_ptr<Object>& object, glm::vec3 position, float rotation, glm::vec2 scale)
{
    b2BodyDef bodyDef;
    bodyDef.type = object->bodyType; // Use the same body type as the original object
    bodyDef.position.Set(position.x, position.y);
    bodyDef.angle = rotation;

    // Create the new body
    b2Body* newBody = globalWorld->CreateBody(&bodyDef);

    for (b2Fixture* fixture = object->body->GetFixtureList(); fixture; fixture = fixture->GetNext())
    {
        b2FixtureDef fixtureDef;
        fixtureDef.shape = fixture->GetShape(); // Copy the shape
        fixtureDef.density = fixture->GetDensity();
        fixtureDef.friction = fixture->GetFriction();
        fixtureDef.restitution = fixture->GetRestitution();
        fixtureDef.isSensor = fixture->IsSensor();

        newBody->CreateFixture(&fixtureDef);
    }

    // Create a copy of the current object
    auto newObject = std::make_shared<Object>(
        newBody, 
        object->texture,
        scale,
        object->bodyType,
        rotation,
        position,
        object->color
    );

    // Copy the custom properties
    newObject->isFlipped = object->isFlipped;
    newObject->texturePath = object->texturePath;
    newObject->isRope = object->isRope;
    newObject->notLoadedCustomShape = object->notLoadedCustomShape;
    newObject->isPlaying = object->isPlaying;
    newObject->shape = object->shape;

    // Copy animations
    newObject->animations = object->animations;

    objects.push_back(newObject);

    return newObject;
}

void ObjectManager::HandleMouseInteractions(GLFWwindow* window, Camera& camera) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    glm::vec2 mousePos = globalCamera->screenToWorldSpace(glm::vec2(mouseX, mouseY));

    if (GetMouseButton(MouseKey::LeftClick)) // Left mouse button for dragging
    {
        if (!isDraggingVertex)
        {
            // Check if a vertex is under the mouse cursor
            for (size_t i = 0; i < objects[selectedObjectIndex]->shape.vertices.size(); ++i)
            {
                if (glm::distance(objects[selectedObjectIndex]->shape.vertices[i], mousePos) < 0.1f)
                {
                    selectedVertexIndex = i;
                    isDraggingVertex = true;
                    break;
                }
            }
        }
        else
        {
            // Drag the selected vertex
            objects[selectedObjectIndex]->shape.vertices[selectedVertexIndex] = mousePos;
            objects[selectedObjectIndex]->CreateCustomCollider(objects[selectedObjectIndex]->body);
        }
    }
    else
    {
        isDraggingVertex = false;
        selectedVertexIndex = -1;
    }

    if (GetMouseButtonDown(MouseKey::RightClick)) // Right mouse button for creating new vertex
    {
        if (selectedObjectIndex != -1)
        {
            objects[selectedObjectIndex]->shape.vertices.push_back(mousePos);
            objects[selectedObjectIndex]->CreateCustomCollider(objects[selectedObjectIndex]->body);
        }
    }
}

int ObjectManager::FindObjectIndex(std::shared_ptr<Object> obj) {
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        return std::distance(objects.begin(), it);
    }
    return -1;
}

std::vector<std::filesystem::directory_entry> ObjectManager::ListFilesInDirectory(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions) {
    std::vector<std::filesystem::directory_entry> files;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_directory() || std::any_of(extensions.begin(), extensions.end(), [&entry](const std::string& ext) { return entry.path().extension() == ext; }) || extensions.empty()) {
            files.push_back(entry);
        }
    }

    return files;
}

// Function to display directory contents recursively with multiple extensions
void ObjectManager::DisplayDirectoryContents(const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions) {
    auto files = ListFilesInDirectory(directoryPath, extensions);

    for (const auto& entry : files) {
        if (entry.is_directory()) {
            if (ImGui::TreeNode(entry.path().filename().string().c_str())) {
                DisplayDirectoryContents(entry.path(), extensions);
                ImGui::TreePop();
            }
        }
        else {
            ImGui::Text("%s", entry.path().filename().string().c_str());
        }
    }
}

// Function to create the ImGui file browser window with multiple extensions
void ObjectManager::ShowFileBrowser(bool* p_open) {
    if (!ImGui::Begin("File Browser", p_open)) {
        ImGui::End();
        return;
    }

    const std::filesystem::path directoryPath = "Resources";
    std::vector<std::string> extensions = { ".wav", ".mp3", ".png", ".ttf"};
    DisplayDirectoryContents(directoryPath, extensions);

    ImGui::End();
}

void ObjectManager::PlayScene()
{
    auto camera = ScriptFactory::Instance().GetManager()->globalCamera;
    camera->LoadOriginal();

    for (auto object : objects)
    {
        object->isPlaying = true;
    }

    if (scenes.size() == 0)
    {
        scenes.push_back("MainScene.json");
        selectedSceneIndex = 0;
        SaveObjects(scenes[selectedSceneIndex]);
        LoadObjects(scenes[selectedSceneIndex], *globalWorld);
    }

    if (!playMode)
    {
        SaveObjects(scenes[selectedSceneIndex]);
    }
}

void ObjectManager::StopScene(b2World& world)
{
    for (auto object : objects)
    {
        object->isPlaying = false;
    }

    ScriptFactory::Instance().GetCanvas()->Clear();

    if (!playMode)
    {
        LoadObjects(scenes[selectedSceneIndex], world);
    }
}

void ObjectManager::SaveCamera() const
{
    std::filesystem::path directory = "Data";
    std::filesystem::create_directories(directory);

    std::filesystem::path filePath = directory / "Camera.json";

    nlohmann::json j;

    for (int i = 0; i < cameraSaves.size(); ++i)
    {
        j.push_back(cameraSaves[i].CamToJson());
    }

    std::ofstream file(filePath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
    else {
        std::cerr << "Could not open file for writing: " << filePath << std::endl;
    }
}

void ObjectManager::LoadCamera() {
    std::filesystem::path directory = "Data";
    std::filesystem::path filePath = directory / "Camera.json";

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << "Camera.json" << std::endl;
        return;
    }

    nlohmann::json j;
    file >> j;

    cameraSaves.clear();

    for (auto& camJson : j) {
        CameraSave save;
        save.JsonToCam(camJson);
        cameraSaves.push_back(save);
    }
}