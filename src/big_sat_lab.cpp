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

	_camera.pos = glm::vec3{ 0.0, 0.0, 0.0 };

	PxMaterial* physMat{ engine.create_physics_material(0.5, 0.5, 0.6) };
	PxShape* earthShape{ engine.create_physics_shape(PxSphereGeometry{ 6.371f }, *physMat) };
	PxShape* sunShape{ engine.create_physics_shape(PxSphereGeometry{ 696.34f }, *physMat) };
	PxShape* moonShape{ engine.create_physics_shape(PxSphereGeometry{ 1.7371f }, *physMat) };
	PxShape* satShape{ engine.create_physics_shape(PxCapsuleGeometry{1.0, 1.1}, *physMat) };


	// "earth" is the name of the model's folder in assets/models and also the name of a material in shaders/_load_materials.txt.
	// If these names did not match, we could specify the materials name as a second argument to VulkanEngine::create_render_objects()
	_earth.setRenderObject(engine.create_render_object("earth"));
	_earth.setPos(glm::vec3(-2.5, 100.0, 0.4));
	_earth.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	engine.add_to_physics_engine_dynamic_mass(&_earth, earthShape, 5.972e2f);
	_gravityObjects.push_back(_earth);

	_sun.setRenderObject(engine.create_render_object("sun"));
	_sun.setPos(glm::vec3(-2.5, 7.0, 800.0f));
	_sun.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	engine.add_to_physics_engine_dynamic_mass(&_sun, sunShape, 1.989e8f);
	_gravityObjects.push_back(_sun);

	_moon.setRenderObject(engine.create_render_object("moon"));
	_moon.setPos(glm::vec3(25.0, 25.0, 0.4));
	_moon.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
	//engine.add_to_physics_engine_dynamic_mass(&_moon, moonShape, 7.35e0f);
	//_gravityObjects.push_back(_moon);
	//_moon.setVelocity(glm::vec3{ 0.0, 0.0, 0.0 });

	_sat01.setRenderObject(engine.create_render_object("sat01"));
	_sat01.setPos(_camera.pos + glm::vec3(-10.0, 0.0, 0.0));
	engine.add_to_physics_engine_dynamic_mass(&_sat01, satShape, 1.0f);
	_gravityObjects.push_back(_sat01);

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
}

void BigSatLab::lookatSatellite(VulkanEngine& engine)
{
	float distance{ 10.0f };
	glm::vec3 diff{ _sat01.getPos() - _earth.getPos() };
	glm::vec3 n{ glm::normalize(diff) };
	glm::vec3 camPos{ _sat01.getPos() + n * distance };

	glm::vec3 u{ 0.0, 1.0, 1.0 };
	u.x = -(u.y * n.y + u.z + n.z) / n.x;
	u = glm::normalize(u);

	// glm::lookAt returns the view matrix for both translation and rotation.
	// So we zero out the translation portion of the matrix and inverse the remaining rotation portion
	// since the camera's transform is the inverse of the view matrix
	_camera.rot = glm::lookAt(_camera.pos, _moon.getPos(), glm::vec3{ 0.0, 1.0, 0.0 });
	_camera.rot[3][0] = 0.0;
	_camera.rot[3][1] = 0.0;
	_camera.rot[3][2] = 0.0;
	_camera.rot = glm::inverse(_camera.rot);
	//_camera.pos = camPos;

	//_moon.setPos(camPos);


}

// This is called once per frame
void BigSatLab::update(VulkanEngine& engine, float delta)
{
	_moon.setPos(glm::vec3{ _moonx, _moony, _moonz });

	lookatSatellite(engine);
	//updateCamera(engine);
	engine.set_camera_transform(_camera);
	engine.set_scene_lights(_lights);
	_time += delta;
}

// This is called once at each physics step
void BigSatLab::fixedUpdate(VulkanEngine& engine)
{
	for (auto i{ 0 }; i < _gravityObjects.size(); ++i) {
		glm::vec3 netForce{ 0.0 };

		for (auto j{ 0 }; j < _gravityObjects.size(); ++j) {
			if (j != i) {
				glm::vec3 diff{ _gravityObjects[j].getPos() - _gravityObjects[i].getPos() };
				float r{ glm::length(diff) };
				glm::vec3 direction{ glm::normalize(diff) };
				float magnitude{ (_G * _gravityObjects[i].getMass() * _gravityObjects[j].getMass()) / (r * r) };

				glm::vec3 force{ direction * magnitude };
				netForce += force;
			}
		}

		_gravityObjects[i].addForce(netForce);
	}

	//_earth.addForce(glm::vec3{0.0, 1000.0 * _testFloat, 0.0});
	_sat01.addForce(_satForce);
	_sat01.addTorque(_satTorque);
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

	glm::vec4 camTranslate{ 0.0f };

	// continuous-response keys
	if (keystate[SDL_SCANCODE_W]) {
		camTranslate.z -= speed * delta;
	}
	if (keystate[SDL_SCANCODE_A]) {
		camTranslate.x -= speed * delta;
	}
	if (keystate[SDL_SCANCODE_S]) {
		camTranslate.z += speed * delta;
	}
	if (keystate[SDL_SCANCODE_D]) {
		camTranslate.x += speed * delta;
	}
	if (keystate[SDL_SCANCODE_E]) {
		camTranslate.y += speed * delta;
	}
	if (keystate[SDL_SCANCODE_Q]) {
		camTranslate.y -= speed * delta;
	}

	_camera.pos += glm::vec3{ _camera.rot * camTranslate };

	glm::vec4 satForce{ 0.0f };
	glm::vec4 satTorque{ 0.0f };
	float force{ 10.0f };
	float torque{ 10.0f };

	// torque:
	// x: pitch
	// y: yaw
	// z: roll

	// Control satellite
	if (keystate[SDL_SCANCODE_LEFT]) {
		satTorque.y += torque;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		satTorque.y -= torque;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		satTorque.x += torque;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		satTorque.x -= torque;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		satForce.z += force;
	}

	_satForce = glm::vec3{ _sat01.getRot() * satForce };
	_satTorque = glm::vec3{ _sat01.getRot() * satTorque };

	return bQuit;
}

// This is called once per frame to update GUI
void BigSatLab::gui()
{
	//ImGui::ShowDemoWindow();
	ImGui::DragFloat("moon x", &_moonx, 0.005f);
	ImGui::DragFloat("moon y", &_moony, 0.005f);
	ImGui::DragFloat("moon z", &_moonz, 0.005f);
}
