#include <iostream>

#include "big_sat_lab.h"
#include "../vulkan-renderer/src/vk_engine.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

int main(int argc, char* argv[])
{
#ifdef TRACY_ENABLE
	std::cout << "TRACY_ENABLE is defined\n";
#endif

	VulkanEngine engine;

	BigSatLab app{};

	engine.init(&app);

	engine.run();

	engine.cleanup();

	return 0;
}
