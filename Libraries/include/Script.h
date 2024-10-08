#ifndef SCRIPT_H
#define SCRIPT_H

#include <json.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <memory>

class Object;

// Base script class, meant to be derived off of.
class Script
{
public:
	Script() = default; // Default constructor
	virtual ~Script() = default; // Virtual destructor for polymorphism

	virtual nlohmann::json toJson() const = 0;
	virtual void fromJson(const nlohmann::json& j) = 0; 
	virtual std::string getTypeName() const = 0; // Getting the class name of the script.

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void DrawImGui() = 0;

	void SetObject(std::shared_ptr<Object> obj) { scriptObject = obj; }

	bool startFrameEnded = false;

	// Weak pointer to the object this script is attached to.
	std::weak_ptr<Object> scriptObject;

	// Dictates whether the update and start methods will run in the editor, or not.
	bool editorScript = false;
};

#endif