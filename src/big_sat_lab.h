#pragma once
#include "../vulkan-renderer/src/vk_engine.h"
#include "../vulkan-renderer/src/application.h"

class BigSatLab : public Application {
public:
	void init(VulkanEngine& engine) override;

	void update(VulkanEngine& engine, float delta) override;

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

	// objects
	GameObject _sofa{};
};