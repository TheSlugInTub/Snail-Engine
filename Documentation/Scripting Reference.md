# Scripting Reference

It is important that you have a strong grasp of C++ and pointers before you use Snail Engine.
Snail Engine uses GLM (OpenGL Mathematics) as it's math library.

Scripts can be made in whatever code editor you want to make them in. Just be sure to compile the engine, once you've made them.

You can make a script by deriving off the base script class which has a couple of virtual functions.
This is the template of any script:

```cpp
#pragma once

#include <Snail.h>

class ExampleScript : public Script
{
public:
    ExampleScript(); //Constructor
    ~ExampleScript() override; //Destructor

    nlohmann::json toJson() const override; //Serializing script variables
    void fromJson(const nlohmann::json& j) override; //Deserializing from JSON
    std::string getTypeName() const override; //This should always return the name of the script class

    void Start() override; //Start frame
    void Update() override; //Called every frame
    void DrawImGui() override; //Used for displaying variables in the editor with ImGui

    //Feel free to put any variables or functions you want here
};
```

And you can define all the functions inside a .cpp file, and BE SURE to call the `RegisterScript(  //scriptClass )` macro at the end of the .cpp file.

## Console Logging

You can use the `Console::Log()` function to display anything to the console, it takes in a std::string.

```cpp
Console::Log("I just logged all over the place");
```

## Object Manager

The object manager is an extremely important class that handles all of the objects in a scene, it also renders all the editor GUI (partly).

To get access to it, you can use the `ScriptFactory` class.

```cpp
ObjectManager* manager = ScriptFactory::Instance().GetManager();
```

#### Finding Objects by Name

You can find any object by it's name by using the `FindObjectByName()` function in the object manager.

```cpp
std::shared_ptr<Object> object = manager->FindObjectByName("Player");
```

Since all the objects are shared pointers you can do whatever you like to this object within the script.
If two objects have the same name, this function will return the object that is prior in the std::vector of objects in the object manager.

#### Removing Objects

You can use the `RemoveObject()` function to remove an object. It takes in a shared pointer to an object, and will find this shared pointer in the std::vector of objects that the object manager keeps.

```cpp
std::shared_ptr<Object> obj = manager->FindObjectByName("Enemy");

manager->RemoveObject(obj);
```

#### Global World

The global b2World, which will be mentioned later, is stored in the object manager.

```cpp
b2World* world = manager->globalWorld;
```

Besides these functions and variables, you shouldn't call any other function in the object manager as they are used in the core engine.

## ImGui

The engine uses ImGui as it's UI framework to display the editor, every script that you add to any object in the editor has a space for it's own `DrawImGui()` function.
You must at least have a very basic knowledge of ImGui to draw something in this function. Here's a basic example:

```cpp
ExampleScript::DrawImGui()
{
	ImGui::Text("This is an example script"); //Displaying some basic text
	ImGui::Seperator(); //This is a seperator which is a just a line in the UI which makes things look nicer.

	if (ImGui::Button("Click me!")) //Button
	{
		Console::Log("You clicked me!");
	}

	float x = 0.0f; //Be sure to set this to 0, ImGui displays some weird values if you don't set this to anything

	ImGui::DragFloat("Drag this number", &x); //ImGui takes in a pointer to any value, since it needs to modify it
}
```

It's important to remember that nearly every function to display an interactable UI component in ImGui returns a bool.
This bool will be true if you're interacting with that component, and it will return false if you're not.

## Input

You can get keyboard input by using the `GetKeyDown()` and `GetKey()` functions.

```cpp
if (GetKeyDown(Key::G))
{
	Console::Log("You just pressed the G key!");
}
```

The `GetKeyDown()` function will only return true for the first frame that you press a key, the `GetKey()` function will return true for every frame that you hold down the key.

```cpp
if (GetKey(Key::G))
{
	Console::Log("You are pressing down the G key!");
}
```

You can get mouse input with different functions.

```cpp
if (GetMouseButtonDown(MouseKey::Middle))
{
	Console::Log("You just pressed down middle click!");
}

if (GetMouseButton(MouseKey::Middle))
{
	Console::Log("You are pressing down middle click!");
}
```

You can get the cursor position using the `GetMouseInputHorizontal()` and `GetMouseInputVertical()` functions.

```cpp
double x = GetMouseInputHorizontal();
double y = GetMouseInputVertical();
```

[Controller input not finished yet]

## Camera

You can get the global camera using the global camera variable in the object manager.

```cpp
Camera* camera = manager->globalCamera;
```

You can modify it's position but this class has two useful functions. There is a `screenToWorldSpace()` function, and there is a `worldToScreenSpace()` function. Here is an example of it's use:

```cpp
//Get the mouse position
double x = GetMouseInputHorizontal();
double y = GetMouseInputVertical();

//Turn the position into world space
glm::vec3 worldPosition = camera->screenToWorldSpace(glm::vec2(x, y));

//Turn the world position back into screen space
glm::vec2 screenPosition = camera->worldToScreenSpace(worldPosition);
```

## Event System

All buttons in the engine are connected to the event system. You can assign an event name to the button in the editor, and in another script,
you can define the function and register the event in the event system. Here's how you can get acess to the event system:

```cpp
EventSystem* eventSystem = ScriptFactory::Instance().GetEventSystem();
```
And here's how you can register an event.

```cpp
void CallbackEvent() //Make the callback function
{
	Console::Log("Event called!");
}

//Register the callback in the list of events
eventManager->RegisterCallback("Test", CallbackEvent);

//Trigger the event so that CallbackEvent() is called.
eventManager->TriggerEvent("Test");
```

For buttons, the triggering of the event is called for you, you just need to set the event name in the editor,
the same as the event name in the code which in this example is "Test".

## Physics

Snail Engine uses Box2D as it's physics engine. Every object in the scene has a b2Body, which is a Box2D rigidbody.
Here is an example of moving an object by moving it's b2Body.

```cpp
b2Vec2 pos = object->body->GetPosition();
pos.x += 0.4 * MovementSpeed;
object->body->SetTransform(pos, object->body->GetAngle());
/*
This is here for making the b2Body awake, if you just set the transform, 
the b2Body becomes static and doesn't respond to collisions. 
So just add a tiny little force to wake the body up.
*/
b2Vec2 force(0.00001f, 0.0f);
object->body->ApplyForceToCenter(force, true); 
```

The code above me uses several functions from Box2D, and makes the object move a little bit in the X direction.
You'll have to look into the Box2D documentation to fully grasp how to do physics stuff in the engine.