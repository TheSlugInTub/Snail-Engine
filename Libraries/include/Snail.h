#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/norm.hpp>

#include <Window.h>
#include <SnailFunctions.h>
#include <Shader.h>
#include <Camera.h>
#include <stb_image.h>
#include <ObjectManager.h>
#include <Object.h>
#include <Renderer.h>
#include <ScriptFactory.h>
#include <Console.h>
#include <ContactListener.h>