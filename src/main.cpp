#include <iostream>

#include "../vulkan-renderer/src/vk_engine.h"

int main(int argc, char* argv[])
{
	std::cout << "Hello world!\n";

	VulkanEngine engine;

	engine.init();
	
	engine.run();

	engine.cleanup();

	return 0;
}
