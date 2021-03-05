#include "big_sat_lab.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "imgui.h"
#include "SDL.h"

#include <cmath>
#include <iostream>

// This is called once at the start of the program
void BigSatLab::init(VulkanEngine& engine)
{
	// Whether or not mouse stays at center of window
	SDL_SetRelativeMouseMode((SDL_bool)_camMouseControls);

	engine.set_gravity(0.0f);

	_camera.pos = glm::vec3{ 0.0, 7.0, 7.0 };

	PxMaterial* physMat{ engine.create_physics_material(0.5, 0.5, 0.6) };
	PxShape* earthShape{ engine.create_physics_shape(PxSphereGeometry{ 6.371f }, *physMat) };
	PxShape* sunShape{ engine.create_physics_shape(PxSphereGeometry{ 696.34f }, *physMat) };
	PxShape* moonShape{ engine.create_physics_shape(PxSphereGeometry{ 1.7371f }, *physMat) };

	// "earth" is the name of the model's folder in assets/models and also the name of a material in shaders/_load_materials.txt.
	// If these names did not match, we could specify the materials name as a second argument to VulkanEngine::create_render_objects()
	_earth.setRenderObject(engine.create_render_object("earth"));
	_earth.setPos(glm::vec3(-2.5, 7.0, 0.4));
	_earth.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	engine.add_to_physics_engine_dynamic_mass(&_earth, earthShape, 5.972e2f);
	_planets.push_back(_earth);

	_sun.setRenderObject(engine.create_render_object("sun"));
	_sun.setPos(glm::vec3(-2.5, 7.0, 800.0f));
	_sun.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	engine.add_to_physics_engine_dynamic_mass(&_sun, sunShape, 1.989e8f);
	_planets.push_back(_sun);

	_moon.setRenderObject(engine.create_render_object("moon"));
	_moon.setPos(glm::vec3(25.0, 25.0, 0.4));
	_moon.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	engine.add_to_physics_engine_dynamic_mass(&_moon, moonShape, 7.35e0f);
	_moon.setVelocity(glm::vec3{ 2.0, 0.0, 0.0 });
	_planets.push_back(_moon);

	Light light{};
	light.color = glm::vec4{ 1.0, 0.8, 1.0, 1.0e6 }; // w component is intensity
	light.position = glm::vec4{ _sun.getPos(), 0.0 }; // w component is ignored
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
	//glm::vec3 pos{ _earth.getPos() };
	//pos.y = std::sinf(_time);
	//pos.z = std::cosf(_time);
	//pos *= _testFloat;
	//_earth.setPos(pos);

	updateCamera(engine);
	engine.set_scene_lights(_lights);
	_time += delta;
}

// This is called once at each physics step
void BigSatLab::fixedUpdate(VulkanEngine& engine)
{
	for (auto i{ 0 }; i < _planets.size(); ++i) {
		glm::vec3 netForce{ 0.0 };

		for (auto j{ 0 }; j < _planets.size(); ++j) {
			if (j != i) {
				glm::vec3 diff{ _planets[j].getPos() - _planets[i].getPos() };
				float r{ glm::length(diff) };
				glm::vec3 direction{ glm::normalize(diff) };
				float magnitude{ (_G * _planets[i].getMass() * _planets[j].getMass()) / (r * r) };

				glm::vec3 force{ direction * magnitude };
				netForce += force;
			}
		}

		_planets[i].addForce(netForce);
	}

	//_earth.addForce(glm::vec3{0.0, 1000.0 * _testFloat, 0.0});
	_moon.addForce(glm::vec3{ 1000.0 * _testFloat, 0.0, 0.0 });
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
