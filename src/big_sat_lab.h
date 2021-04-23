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

	void updateCameraOrbit(VulkanEngine& engine);

	void lookatSatellite(VulkanEngine& engine);

private:
	// Camera variables
	Transform _camera{};
	float _camRotPhi{};
	float _camRotTheta{};
	bool _camMouseControls{ false };

	float _camOrbitPhi{};
	float _camOrbitTheta{};
	float _camOrbitRadius{ 10.0f };

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

	//imgui vars
	float _moonx{ 0.0f };
	float _moony{ 0.0f };
	float _moonz{ -8.0f };
};
