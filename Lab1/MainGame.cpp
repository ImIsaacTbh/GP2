#include "MainGame.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include "IMGUI/imgui_impl_sdl2.h"
#include "IMGUI/imgui_impl_opengl3.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dirent.h>
#endif

unsigned int indices[] = { 0, 1, 2 };
Transform transform;
GLuint defaultWhiteTexture = 0;

void CreateDefaultWhiteTexture()
{
	unsigned char white[4] = { 255, 255, 255, 255 };
	
	glGenTextures(1, &defaultWhiteTexture);
	glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

MainGame::MainGame()
{
}

MainGame::~MainGame()
{
	_gameState = GameState::PLAY;
	if (defaultWhiteTexture != 0)
	{
		glDeleteTextures(1, &defaultWhiteTexture);
	}
}

double gimmenumber()
{
	return static_cast<double>(rand()) / RAND_MAX;
}

void MainGame::run()
{
	srand(time(NULL));
	initSystems();
	counter = 0;
	vector<Vertex> things = {
		//Vertex(glm::vec3(0, 0.5, 0), glm::vec2(0.0, 0.0)),
		//Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(0.0, 1.0)),
		//Vertex(glm::vec3(0.5, -0.5, 0), glm::vec2(1.0, 0.0))
	};
	gameLoop(); 
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay();
	SDL_SetRelativeMouseMode(SDL_TRUE);
	
	// Create default white texture for missing textures
	CreateDefaultWhiteTexture();

	glGenFramebuffers(1, &fbo);

	for(int i = 0; i < 4; i++)
	{
		glGenFramebuffers(1, &shadowFBOs[i]);
		glGenTextures(1, &shadowMaps[i]);
		glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// pretend this is far more clean than it is pls
	objects.push_back(*(new _object{0,
		Transform(glm::vec3(2, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/texture.png"), "../Lab1/Resources/monki.obj"
		}));

	objects.push_back(*(new _object{1,
		Transform(glm::vec3(-2, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/funnyColourThing", ""), "../Lab1/Resources/monki.obj"
		}));

	objects.push_back(*(new _object{2,
		Transform(glm::vec3(0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/funnyColourThing", ""), "../Lab1/Resources/cube.obj"
		}));

	objects.push_back(*(new _object{3,
		Transform(glm::vec3(0, 2, 0), glm::vec3(0.0, 0.0, glm::pi<float>()), glm::vec3(30, 1, 30)),
		new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg"), "../Lab1/Resources/plane.obj"
		}));

	/*objects.push_back(*(new _object{ 4,
		Transform(glm::vec3(0), glm::vec3(0), glm::vec3(-30, -30, -30)),
		new Mesh("../Lab1/Resources/colourShader", ""), "../Lab1/Resources/smolSPHERE.obj", glm::vec3(1.0, 0.5, 0.5)}));*/

	myCamera = Camera(glm::vec3(0, 0, -4), 5, 1.777777777777778, 0.1f, 10000);
	shadowShader = new Shader("../Lab1/Resources/shadowDepth");
}

void MainGame::gameLoop()
{
	second = chrono::high_resolution_clock::now();
	framesCompleted = 0;
	while (_gameState != GameState::EXIT)
	{
		if (glm::abs(chrono::duration_cast<chrono::milliseconds>(second - chrono::high_resolution_clock::now()).count() / 1000.0f) > 1)
		{
			std::cout << framesCompleted << std::endl;
			framerate = framesCompleted;
			framesCompleted = 0;
			second = chrono::high_resolution_clock::now();
		}
		processInput();
		startIMGUI();
		drawGame();
		deltaTime = 0;
		deltaTime = glm::abs(chrono::duration_cast<chrono::milliseconds>(prevFrameStart - chrono::high_resolution_clock::now()).count() / 1000.0f);
		prevFrameStart = chrono::high_resolution_clock::now();
		framesCompleted++;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

// for the record it still doesnt run on linux 😭

std::vector<std::string> MainGame::GetAllImagesInResources()
{
	static const std::vector<std::string> imageExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
	std::vector<std::string> results;
	const std::string resourcePath = "../Lab1/Resources/";

#ifdef _WIN32
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA((resourcePath + "*").c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return results;
	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		std::string filename(findData.cFileName);
#else
	DIR* dir = opendir(resourcePath.c_str());
	if (!dir) return results;
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr)
	{
		if (entry->d_type != DT_REG) continue;
		std::string filename(entry->d_name);
#endif
		size_t dotPos = filename.rfind('.');
		if (dotPos == std::string::npos) continue;

		std::string ext = filename.substr(dotPos);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (std::find(imageExtensions.begin(), imageExtensions.end(), ext) != imageExtensions.end())
			results.push_back(resourcePath + filename);

#ifdef _WIN32
	} while (FindNextFileA(hFind, &findData));
	FindClose(hFind);
#else
	}
	closedir(dir);
#endif
	return results;
}

std::vector<std::string> MainGame::GetAllShadersInResources()
{
	std::vector<std::string> results;
	const std::string resourcePath = "../Lab1/Resources/";

#ifdef _WIN32
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA((resourcePath + "*").c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return results;
	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		std::string filename(findData.cFileName);
#else
	DIR* dir = opendir(resourcePath.c_str());
	if (!dir) return results;
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr)
	{
		if (entry->d_type != DT_REG) continue;
		std::string filename(entry->d_name);
#endif
		size_t dotPos = filename.rfind('.');
		if (dotPos == std::string::npos) continue;

		std::string ext = filename.substr(dotPos);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".vert" || ext == ".frag")
		{
			std::string base = resourcePath + filename.substr(0, dotPos);
			if (std::find(results.begin(), results.end(), base) == results.end())
				results.push_back(base);
		}

#ifdef _WIN32
	} while (FindNextFileA(hFind, &findData));
	FindClose(hFind);
#else
	}
	closedir(dir);
#endif
	return results;
}

void MainGame::startIMGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	static char texBuf[256] = "";
	static int lastSelected = -1;

	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	ImGui::Begin("Scene##hierarchy");

	ImGui::Text("Create Primitive");
	if (ImGui::Button("Cube"))			CreatePrimitive(CUBE, "../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg");
	ImGui::SameLine();
	if (ImGui::Button("Sphere"))		CreatePrimitive(ICOSPHERE, "../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg");
	ImGui::SameLine();
	if (ImGui::Button("Plane"))			CreatePrimitive(PLANE, "../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg");
	ImGui::SameLine();
	if (ImGui::Button("Monki"))			CreatePrimitive(MONKI, "../Lab1/Resources/textureShader", "../Lab1/Resources/texture.png");

	if (ImGui::Button("AssadSphere"))	CreatePrimitive(UVSPHERE, "../Lab1/Resources/textureShader", "../Lab1/Resources/assad.png", glm::vec3(2));
	ImGui::SameLine();
	if (ImGui::Button("Lucio"))			CreatePrimitive(LUCIO, "../Lab1/Resources/textureShader", "../Lab1/Resources/BakedShiz.png");

	ImGui::Separator();
	ImGui::Text("Scene Objects (%zu)", objects.size());
	ImGui::BeginChild("ObjectList", ImVec2(0, 0), true);
	for (int i = 0; i < objects.size(); i++)
	{
		if (ImGui::Selectable(objects[i].name, selectedObject == i))
		{
			selectedObject = i;
			lastSelected = -1;
		}
	}
	ImGui::EndChild();

	ImGui::End();

	//stats page
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_Once);
	ImGui::Begin("Stats");

	ImGui::Text(("Framerate: " + to_string(framerate)).c_str());

	ImGui::End();
	//inspector type stuff
	ImGui::SetNextWindowSize(ImVec2(300, 475), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 420), ImGuiCond_Once);
	ImGui::Begin("Inspector");

	if (selectedObject >= 0 && selectedObject < objects.size())
	{
		_object& obj = objects[selectedObject];

		ImGui::InputText("Name", obj.name, sizeof(obj.name));

		//move the bastard
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			glm::vec3 pos = *obj._transform.GetPos();
			glm::vec3 rot = *obj._transform.GetRot();
			glm::vec3 scl = *obj._transform.GetScale();

			if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) obj._transform.SetPos(pos);

			glm::vec3 rotDeg = glm::degrees(rot);
			if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotDeg), 1.0f))
				obj._transform.SetRot(glm::radians(rotDeg));

			static bool lockScale = false;
			static glm::vec3 prevScl = glm::vec3(1.0f);

			if (lastSelected != selectedObject)
				prevScl = *obj._transform.GetScale();

			glm::vec3 prevSclFrame = *obj._transform.GetScale();
			if (ImGui::DragFloat3("Scale", glm::value_ptr(scl), 0.1f))
			{
				if (lockScale)
				{
					glm::vec3 delta = scl - prevSclFrame;
					float driven = (delta.x != 0.0f) ? scl.x : (delta.y != 0.0f) ? scl.y : scl.z;
					scl = glm::vec3(driven);
				}
				obj._transform.SetScale(scl);
			}

			if (lockScale)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.65f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
				if (ImGui::Button("Scale Locked")) lockScale = false;
				ImGui::PopStyleColor(3);
			}
			else
			{
				if (ImGui::Button("Lock Scale Together")) lockScale = true;
			}
		}

		// texture shiz
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Graficks", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static char shaderBuf[256] = "";
			static char texBuf[256] = "";
			static int lastSelected = -1;

			// purge buffers
			if (lastSelected != selectedObject)
			{
				if (obj._mesh->shader != nullptr)
					snprintf(shaderBuf, sizeof(shaderBuf), "%s", obj._mesh->shader->GetPath().c_str());
				else
					shaderBuf[0] = '\0';

				if (obj._mesh->diffuseTex != nullptr)
					snprintf(texBuf, sizeof(texBuf), "%s", obj._mesh->diffuseTex->GetPath().c_str());
				else
					texBuf[0] = '\0';

				lastSelected = selectedObject;
			}

			// cache the lists, only refresh when requested
			static std::vector<std::string> cachedShaders;
			static std::vector<std::string> cachedImages;
			static bool resourcesCached = false;

			if (!resourcesCached)
			{
				cachedShaders = GetAllShadersInResources();
				cachedImages = GetAllImagesInResources();
				resourcesCached = true;
			}

			// shady guy
			ImGui::Text("Shader");
			ImGui::BeginChild("ShaderEdits", ImVec2(0, 75.0f), true);
			if (obj._mesh->shader != nullptr)
				ImGui::TextWrapped("Current: %s", obj._mesh->shader->GetPath().c_str());
			else
				ImGui::TextDisabled("No shader assigned (oh shid)");

			static int selectedShader = -1;
			{
				// build a c-string list for the combo
				std::vector<const char*> shaderItems;
				for (auto& s : cachedShaders) shaderItems.push_back(s.c_str());

				if (ImGui::Combo("##shaderdrop", &selectedShader, shaderItems.data(), (int)shaderItems.size()))
				{
					snprintf(shaderBuf, sizeof(shaderBuf), "%s", cachedShaders[selectedShader].c_str());
					delete obj._mesh->shader;
					obj._mesh->shader = new Shader(shaderBuf);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("R##shaderrefresh")) resourcesCached = false;
			ImGui::EndChild();

			// textur
			ImGui::BeginChild("TextureEdits", ImVec2(0, 100.0f), true);
			ImGui::Text("Texture");
			if (obj._mesh->diffuseTex != nullptr)
				ImGui::TextWrapped("Current: %s", obj._mesh->diffuseTex->GetPath().c_str());
			else
				ImGui::TextDisabled("No texture assigned");

			static int selectedTex = -1;
			{
				std::vector<const char*> texItems;
				for (auto& t : cachedImages) texItems.push_back(t.c_str());

				if (ImGui::Combo("##texdrop", &selectedTex, texItems.data(), (int)texItems.size()))
				{
					snprintf(texBuf, sizeof(texBuf), "%s", cachedImages[selectedTex].c_str());
					delete obj._mesh->diffuseTex;
					obj._mesh->diffuseTex = new Texture(texBuf);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear##tex"))
			{
				delete obj._mesh->diffuseTex;
				obj._mesh->diffuseTex = nullptr;
				selectedTex = -1;
			}
			ImGui::SameLine();
			if (ImGui::Button("R##texrefresh")) resourcesCached = false;
			ImGui::EndChild();
		}

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("Delete Object", ImVec2(-1, 0)))
		{
			delete obj._mesh;
			objects.erase(objects.begin() + selectedObject);
			selectedObject = -1;
			lastSelected = -1;
		}
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::TextDisabled("No object selected");
	}

	ImGui::End();
}

void MainGame::processInput()
{
	bool yesno;
	SDL_Event aaaa;
	while (SDL_PollEvent(&aaaa))
	{
		ImGui_ImplSDL2_ProcessEvent(&aaaa);
		switch (aaaa.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			if (SDL_GetRelativeMouseMode() == SDL_FALSE) break;
			myCamera.Pitch(aaaa.motion.yrel * -0.05f * sens * deltaTime);
			myCamera.RotateY(aaaa.motion.xrel * 0.05f * sens * deltaTime);
			break;
		}
	}

	auto keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_ESCAPE])
	{
		yesno = !(bool)SDL_GetRelativeMouseMode();

		SDL_SetRelativeMouseMode((SDL_bool)(int)yesno);
	}
	if (SDL_GetRelativeMouseMode() == SDL_FALSE) return;
	if (keystate[SDL_SCANCODE_W])
	{
		myCamera.MoveForward(0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		myCamera.MoveForward(-0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_A])
	{
		myCamera.MoveRight(-0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_D]*1000*deltaTime)
	{
		myCamera.MoveRight(0.01*1000*deltaTime);
	}
}

glm::mat4 MainGame::CalculateLightSpaceMatrix()
{
	glm::vec3 lightPos = *lightingTransform.GetPos();
	
	// light perspective
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	// orthographic projection
	float orthonear = 0.1f;
	float orthofar = 1000.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, orthonear, orthofar);
	
	//combine
	return lightProjection * lightView;
}

void MainGame::shadowPass(const std::vector<glm::vec3>& lightPositions)
{
	shadowShader->Bind();

	for (int l = 0; l < lightPositions.size() && l < 4; l++)
	{
		glm::mat4 lightView = glm::lookAt(lightPositions[l], glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, 100.0f, -100.0f, 0.1f, 1000.0f);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[l]);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < objects.size(); i++)
		{
			glm::mat4 mvp = lightSpaceMatrix * objects[i]._transform.GetModel();
			GLint mvpLoc = glGetUniformLocation(shadowShader->program, "lightSpaceMVP");
			if (mvpLoc != -1)
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
			objects[i]._mesh->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainGame::drawAllObjects()
{
	std::vector<glm::vec3> lightPositions = {
		*lightingTransform.GetPos(),
		glm::vec3(30.0f, -5.0f, 30.0f)
	};
	std::vector<glm::vec3> lightColors = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.8f, 0.6f)
	};

	//math ain't mathing anymore lmao
	std::vector<glm::mat4> lightSpaceMatrices;
	for (int l = 0; l < lightPositions.size(); l++)
	{
		glm::mat4 lightView = glm::lookAt(lightPositions[l], glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
		lightSpaceMatrices.push_back(lightProjection * lightView);
	}

	for (int i = 0; i < objects.size(); i++)
	{

		objects[i]._mesh->shader->Bind();
		//light space matrix for transform
		if (objects[i]._basicColor != glm::vec3(0, 0, 0))
		{
			objects[i]._mesh->shader->UpdateWithShadow(objects[i]._transform, myCamera, lightPositions[0], lightSpaceMatrices[0], objects[i]._basicColor);
		}
		else
		{
			objects[i]._mesh->shader->UpdateWithShadow(objects[i]._transform, myCamera, lightPositions[0], lightSpaceMatrices[0]);
		}

		//lights
		objects[i]._mesh->shader->SetLights(lightPositions, lightColors);

		//light space matrices
		objects[i]._mesh->shader->SetLightSpaceMatrices(lightSpaceMatrices);

		if (objects[i]._mesh->diffuseTex != nullptr)
		{
			objects[i]._mesh->shader->SetTexture("diffuse", objects[i]._mesh->diffuseTex->GetID(), 0);
		}

		for (int l = 0; l < lightPositions.size() && l < 4; l++)
		{
			std::string name = "shadowMap[" + std::to_string(l) + "]";
			objects[i]._mesh->shader->SetTexture(name, shadowMaps[l], 3 + l);
		}

		objects[i]._mesh->Draw();
	}
}

void MainGame::drawGame()
{
	lightingTransform.SetPos(glm::vec3(glm::sin(counter) * 10, -glm::abs(glm::sin(counter * 5)), glm::cos(counter) * 10));
	lightingTransform.SetRot(*lightingTransform.GetPos() - glm::vec3(0, 0, 0));

	Transform lightThingTransform;
	lightThingTransform.SetPos(*lightingTransform.GetPos());
	lightThingTransform.SetRot(*lightingTransform.GetRot());
	lightThingTransform.SetScale(glm::vec3(0.5, 0.5, 0.5));

	objects[0]._transform.SetRot(glm::vec3(0.0, counter*2, 0.0));
	objects[1]._transform.SetRot(glm::vec3(0.0, -counter*2, 0.0));
	objects[2]._transform = lightThingTransform;

	// PASS 1: Shadow mapping
	std::vector<glm::vec3> lightPositions = {
	*lightingTransform.GetPos(),
	glm::vec3(0.0f, -5.0f, 0.0f),
	glm::vec3(30.0f, -5.0f, 0.0f)
	};
	shadowPass(lightPositions);

	// PASS 2: Render to fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, *_gameDisplay.GetWidth(), *_gameDisplay.GetHeight());
	glCullFace(GL_BACK);
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// PASS 3: Final render with lighting and shadows to screen and hope imgui doesnt die again :(
	glViewport(0, 0, *_gameDisplay.GetWidth(), *_gameDisplay.GetHeight());
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);
	drawAllObjects();

	counter += 0.01f;

	glEnd();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	_gameDisplay.swapBuffer();
}