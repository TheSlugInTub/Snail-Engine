#pragma once

// Header files to include all other header files in the project

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

#include <Box2D/box2d/box2d.h>
#include <stb_image.h>

#include <Window.h>
#include <SnailFunctions.h>
#include <Shader.h>
#include <Camera.h>
#include <ObjectManager.h>
#include <Object.h>
#include <Renderer.h>
#include <ScriptFactory.h>
#include <Console.h>
#include <ContactListener.h>
#include <Tilemap.h>
#include <SoundBuffer.h>
#include <SoundDevice.h>
#include <SoundSource.h>
#include <Animation.h>
#include <EventSystem.h>