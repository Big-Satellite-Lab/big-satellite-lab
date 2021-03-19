#pragma once
#include "../vulkan-renderer/src/vk_engine.h"
#include "../vulkan-renderer/src/application.h"

#include <vector>

class BigSatLab : public Application {
public:
	void init(VulkanEngine& engine) override;

	void update(VulkanEngine& engine, float delta) override;

	void fixedUpdate(VulkanEngine& engine) override;

	bool input(float delta) override;

	void gui() override;

	// New functions
	void updateCamera(VulkanEngine& engine);

private:
	// Camera variables
	Transform _camera{};
	float _camRotPhi{};
	float _camRotTheta{};
	bool _camMouseControls{ false };

	// elapsed seconds since program start
	float _time{};

	// scene point lights
	std::vector<Light> _lights;

	// objects
	GameObject _earth{};
	GameObject _sun{};
	GameObject _moon{};
	GameObject _mars{};
	std::vector<GameObject> _gravityObjects;

	GameObject _sat01{};
	glm::vec3 _satForce{};
	glm::vec3 _satTorque{};

	// physics vars
	float _G{ 6.674e-4f };

	// test imgui float
	float _testFloat{ 0.0f };
};
