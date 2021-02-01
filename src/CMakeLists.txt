
# Add source to this project's executable.
add_executable (big_satellite_lab 
"main.cpp"
 "../vulkan-renderer/src/vk_engine.cpp"
 "../vulkan-renderer/src/vk_engine.h" 
 "../vulkan-renderer/src/vk_types.h" 
 "../vulkan-renderer/src/vk_initializers.cpp" 
 "../vulkan-renderer/src/vk_initializers.h"
 "../vulkan-renderer/src/vk_mesh.cpp"
 "../vulkan-renderer/src/vk_mesh.h"
 "../vulkan-renderer/src/vk_textures.cpp"
 "../vulkan-renderer/src/vk_textures.h"
 "../vulkan-renderer/src/render_to_texture.h"
 "../vulkan-renderer/src/render_to_texture.cpp"
 "../vulkan-renderer/tracy/TracyClient.cpp"
)


set_property(TARGET big_satellite_lab PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:big_satellite_lab>")

target_include_directories(big_satellite_lab PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(big_satellite_lab  vkbootstrap vma glm tinyobjloader imgui stb_image)

target_link_libraries(big_satellite_lab Vulkan::Vulkan sdl2)

add_dependencies(big_satellite_lab Shaders)