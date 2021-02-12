#include "big_sat_lab.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "imgui.h"
#include "SDL.h"

#include <cmath>

// This is called once at the start of the program
void BigSatLab::init(VulkanEngine& engine)
{
	// Whether or not mouse stays at center of window
	SDL_SetRelativeMouseMode((SDL_bool)_camMouseControls);

	_camera.pos = glm::vec3{ 0.0, 7.0, 7.0 };

	// "earth" is the name of the model's folder in assets/models and also the name of a material in shaders/_load_materials.txt.
	// If these names did not match, we could specify the materials name as a second argument to VulkanEngine::create_render_objects()
	_earth.setRenderObject(engine.create_render_object("earth"));
	_earth.setPos(glm::vec3(-2.5, 0.0, 0.4));
	_earth.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));

	_moon.setRenderObject(engine.create_render_object("moon"));
	_moon.setPos(glm::vec3(-5.0, 0.0, 0.4));
	_moon.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));

	Light light{};
	light.color = glm::vec4{ 1.0, 0.8, 1.0, 100.0 }; // w component is intensity
	light.position = glm::vec4{ 1.0, 8.0, 4.0, 0.0 }; // w component is ignored
	// Only lights in this vector will be rendered!
	_lights.push_back(light);
}

void BigSatLab::updateCamera(VulkanEngine& engine)
{
	// Translate the two angles into 4x4 matrices
	glm::mat4 rotTheta{ glm::rotate(_camRotTheta, glm::vec3{ 1.0f, 0.0f, 0.0f }) };
	glm::mat4 rotPhi{ glm::rotate(_camRotPhi, glm::vec3{ 0.0f, 1.0f, 0.0f }) };
	_camera.rot = rotPhi * rotTheta;
	engine.set_camera_transform(_camera);
}

// This is called once per frame
void BigSatLab::update(VulkanEngine& engine, float delta)
{
	glm::vec3 pos{ _earth.getPos() };
	pos.y = std::sinf(_time);
	pos.z = std::cosf(_time);
	pos *= _testFloat;
	_earth.setPos(pos);

	updateCamera(engine);
	engine.set_scene_lights(_lights);
	_time += delta;
}

// This is called once per frame to handle user input
bool BigSatLab::input(float delta)
{
	float speed{ 3.0f };
	float camSensitivity{ 0.3f };
	// mouse motion seems to be sampled 60fps regardless of framerate
	constexpr float mouseDelta{ 1.0f / 60.0f };

	const Uint8* keystate{ SDL_GetKeyboardState(nullptr) };

	bool bQuit{ false };
	SDL_Event e;
	// Handle events on queue
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_f) {
				_camMouseControls = !_camMouseControls;
				SDL_SetRelativeMouseMode((SDL_bool)_camMouseControls);
			}
			break;
		case SDL_MOUSEMOTION:
			if (_camMouseControls) {
				_camRotPhi -= e.motion.xrel * camSensitivity * mouseDelta;
				_camRotTheta -= e.motion.yrel * camSensitivity * mouseDelta;
				_camRotTheta = std::clamp(_camRotTheta, -pi / 2.0f, pi / 2.0f);
			}
			break;
		case SDL_QUIT:
			bQuit = true;
			break;
		}
	}

	glm::vec4 translate{ 0.0f };

	// continuous-response keys
	if (keystate[SDL_SCANCODE_W]) {
		translate.z -= speed * delta;
	}
	if (keystate[SDL_SCANCODE_A]) {
		translate.x -= speed * delta;
	}
	if (keystate[SDL_SCANCODE_S]) {
		translate.z += speed * delta;
	}
	if (keystate[SDL_SCANCODE_D]) {
		translate.x += speed * delta;
	}
	if (keystate[SDL_SCANCODE_E]) {
		translate.y += speed * delta;
	}
	if (keystate[SDL_SCANCODE_Q]) {
		translate.y -= speed * delta;
	}

	_camera.pos += glm::vec3{ _camera.rot * translate };

	return bQuit;
}

// This is called once per frame to update GUI
void BigSatLab::gui()
{
	//ImGui::ShowDemoWindow();
	ImGui::DragFloat("drag float", &_testFloat, 0.005f);
}
