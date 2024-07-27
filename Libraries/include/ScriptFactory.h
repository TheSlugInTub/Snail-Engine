#ifndef SCRIPT_FACTORY_H
#define SCRIPT_FACTORY_H

#include <Script.h>
#include <iostream> // For debug output
#include <ObjectManager.h>
#include <ContactListener.h>
#include <Canvas.h>

class ScriptFactory
{
public:
    using ScriptCreator = std::function<std::unique_ptr<Script>()>;

    static ScriptFactory& Instance()
    {
        static ScriptFactory instance;
        return instance;
    }

    void RegisterScript(const std::string& name, ScriptCreator creator)
    {
        std::cout << "Registering script: " << name << std::endl;
        creators[name] = creator;
    }

    std::unique_ptr<Script> CreateScript(const std::string& name)
    {
        if (creators.find(name) != creators.end())
        {
            return creators[name]();
        }
        return nullptr;
    }

    std::vector<std::string> GetRegisteredScriptNames() const
    {
        std::vector<std::string> names;
        for (const auto& pair : creators)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    void SetManager(std::unique_ptr<ObjectManager> manager)
    {
        globalManager = std::move(manager);
    }

    // Get the global object manager
    ObjectManager* GetManager() const
    {
        return globalManager.get();
    }

    void SetRenderer(std::unique_ptr<Renderer> manager)
    {
        globalRenderer = std::move(manager);
    }

    // Get the global object manager
    Renderer* GetRenderer() const
    {
        return globalRenderer.get();
    }

    void SetListener(CollisionListener& manager)
    {
        globalListener = &manager;
    }

    // Get the global object manager
    CollisionListener* GetListener() const
    {
        return globalListener;
    }

    void SetCanvas(Canvas& manager)
    {
        globalCanvas = &manager;
    }

    // Get the global object manager
    Canvas* GetCanvas() const
    {
        return globalCanvas;
    }

private:
    ScriptFactory() = default;
    std::unordered_map<std::string, ScriptCreator> creators;
    std::unique_ptr<ObjectManager> globalManager;
    std::unique_ptr<Renderer> globalRenderer;
    CollisionListener* globalListener;
    Canvas* globalCanvas;
};

// Macro for registering scripts
#define REGISTER_SCRIPT(scriptClass) \
    static bool scriptClass##_registered = []() { \
        std::cout << "Executing REGISTER_SCRIPT for: " << #scriptClass << std::endl; \
        ScriptFactory::Instance().RegisterScript(#scriptClass, []() { \
            return std::make_unique<scriptClass>(); \
        }); \
        return true; \
    }();

#endif