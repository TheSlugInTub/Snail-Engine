#include <Snail.h>
#include <string>
#include <thread>

void processInput(GLFWwindow* window);

bool worldStep;
float timeStep = 1.0f / 15.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;

const unsigned int screenWidth = 1920;
const unsigned int screenHeight = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

float lastX = (float)screenWidth / 2.0;
float lastY = (float)screenHeight / 2.0;
bool firstMouse = true;

std::string FPS;
auto lastTime = std::chrono::high_resolution_clock::now();
int frameCount = 0;
float fps = 0.0f;

bool started;
bool playMode = false;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (state == GLFW_PRESS)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

int main()
{
	GLFWwindow* window = initializeWindow("Snail Engine", screenWidth, screenHeight, false);
	setWindowIcon(window, "Resources/Textures/Slugarius.png");
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSwapInterval(1); 

	camera.SaveOriginal();
	camera.SaveFrustumCorners();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    
	ImFont* mainfont = io.Fonts->AddFontFromFileTTF("Resources/Fonts/newfont.ttf", 18.5f);
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	b2World world(b2Vec2(0.0f, -0.5f));
	
	std::unique_ptr<ObjectManager> objectManager = std::make_unique<ObjectManager>();
	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	CollisionListener* collisionListener = new CollisionListener();
	Canvas canvas(screenWidth, screenHeight);
	ScriptFactory::Instance().SetManager(std::move(objectManager));
	ScriptFactory::Instance().SetRenderer(std::move(renderer));
	ScriptFactory::Instance().SetListener(*collisionListener);
	ScriptFactory::Instance().SetCanvas(canvas);
	world.SetContactListener(collisionListener);

	double prevTime = 0.0;
	double currentTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - lastTime;
		lastTime = currentTime;

		// Update frame count
		frameCount++;

		// Accumulate time
		static float timeAccumulator = 0.0f;
		timeAccumulator += elapsed.count();

		// Update FPS every second
		if (timeAccumulator >= 0.1f) {
			fps = frameCount / timeAccumulator;

			// Reset counters
			frameCount = 0;
			timeAccumulator = 0.0f;

			// Update the FPS string
			FPS = std::to_string(fps);
		}

		//Clear screen.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Main loop.
		processInput(window);

		if (worldStep) { world.Step(timeStep, velocityIterations, positionIterations); }

		ObjectManager* objectManager = ScriptFactory::Instance().GetManager();
		Renderer* renderer = ScriptFactory::Instance().GetRenderer();

		if (objectManager)
		{
			objectManager->UpdateAllTransforms();
			objectManager->RenderAll(camera, *renderer);

			if (!started)
			{
				objectManager->SetAllResources();
				objectManager->SetWorld(world);

				if (playMode)
				{
					objectManager->LoadScenes();
					objectManager->selectedSceneIndex = 0;
					objectManager->LoadObjects(objectManager->scenes[objectManager->selectedSceneIndex], world);
					objectManager->PlayScene();
					objectManager->playMode = true;
					objectManager->SaveObjects(objectManager->scenes[objectManager->selectedSceneIndex]);
					worldStep = true;
				}

				started = true;
			}
		}

		if (GetKeyDown(Key::Escape))
		{
			glfwSetWindowShouldClose(window, true);
		}

		Canvas* canvas = ScriptFactory::Instance().GetCanvas();
		canvas->RenderCanvas();

		if (!playMode)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::PushFont(mainfont);

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			window_flags |= ImGuiWindowFlags_NoBackground; // No background

			// Dockspace flags
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

			// Begin dockspace
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", nullptr, window_flags);
			ImGui::PopStyleVar();

			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			ImGui::End(); // End dockspace window

			ImGui::Begin("File");
			if (!worldStep)
			{
				if (ImGui::Button("Play"))
				{
					objectManager->PlayScene();
					worldStep = true;
				}
			}
			else
			{
				if (ImGui::Button("Stop"))
				{
					objectManager->StopScene(world);
					worldStep = false;
				}
			}

			ImGui::Separator();

			if (!worldStep)
			{
				if (ImGui::Button("Save"))
				{
					if (objectManager->scenes.size() == 0)
					{
						objectManager->scenes.push_back("MainScene.json");
						objectManager->selectedSceneIndex = 0;
						objectManager->SaveObjects(objectManager->scenes[objectManager->selectedSceneIndex]);
						objectManager->LoadObjects(objectManager->scenes[objectManager->selectedSceneIndex], world);
					}
					else
					{
						if (objectManager)
							objectManager->SaveObjects(objectManager->scenes[objectManager->selectedSceneIndex]);
					}
				}

				if (ImGui::Button("Load"))
				{
					objectManager->LoadScenes();
					objectManager->selectedSceneIndex = 0;
					objectManager->LoadObjects(objectManager->scenes[objectManager->selectedSceneIndex], world);
					objectManager->SaveObjects(objectManager->scenes[objectManager->selectedSceneIndex]);
				}
			}

			ImGui::Text("FPS: ");
			ImGui::Text(FPS.c_str());
			if (ImGui::Button("Update resources."))
			{
				objectManager->SetAllResources();
			}
			ImGui::DragFloat("Time step", &timeStep);
			if (ImGui::DragFloat3("Camera position", glm::value_ptr(camera.OriginalPosition)))
			{
				camera.SaveFrustumCorners();
			}
			ImGui::End();

			Console::Draw();
		
			if (objectManager)
				objectManager->DrawImGui(world);

			ImGui::PopFont();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
		}

		//End of main loop.
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//End of program.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::UP, 0.03f);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::DOWN, 0.03f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::LEFT, 0.03f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::RIGHT, 0.03f);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::FORWARD, 0.03f);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraMovement::BACKWARD, 0.03f);
}