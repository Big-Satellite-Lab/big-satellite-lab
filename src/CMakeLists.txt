
# Add source to this project's executable.
add_executable (big_satellite_lab 
 "main.cpp"
 "big_sat_lab.h"
 "big_sat_lab.cpp"
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
 "../vulkan-renderer/src/application.h"
 "../vulkan-renderer/src/physics.h"
 "../vulkan-renderer/src/physics.cpp"
 "${sdl2_DIR}"
)


set_property(TARGET big_satellite_lab PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:big_satellite_lab>")


target_include_directories(big_satellite_lab PUBLIC
 "${CMAKE_CURRENT_SOURCE_DIR}"
 "${CMAKE_CURRENT_SOURCE_DIR}/../vulkan-renderer/third_party/SDL2_mixer-2.0.4/include"
 "${CMAKE_CURRENT_SOURCE_DIR}/../vulkan-renderer/third_party/physx/include"
 "${CMAKE_CURRENT_SOURCE_DIR}/../vulkan-renderer/third_party/physx/include_shared"
)

target_include_directories(big_satellite_lab PUBLIC "${sdl2_DIR}")

target_link_libraries(big_satellite_lab  vkbootstrap vma glm tinyobjloader imgui stb_image)
target_link_libraries(big_satellite_lab ${AUDIO_LIB})
target_link_libraries(big_satellite_lab
 ${PHYSICS_LIB_00}
 ${PHYSICS_LIB_01}
 ${PHYSICS_LIB_02}
 ${PHYSICS_LIB_03}
 ${PHYSICS_LIB_04}
 ${PHYSICS_LIB_05}
 ${PHYSICS_LIB_06}
 ${PHYSICS_LIB_07}
 ${PHYSICS_LIB_08}
 ${PHYSICS_LIB_09}
 ${PHYSICS_LIB_10}
 ${PHYSICS_LIB_11}
#  ${PHYSICS_LIB_12}
#  ${PHYSICS_LIB_13}
#  ${PHYSICS_LIB_14}
 ${PHYSICS_LIB_15}
 ${PHYSICS_LIB_16}
#  ${PHYSICS_LIB_17}
#  ${PHYSICS_LIB_18}
)

target_link_libraries(big_satellite_lab Vulkan::Vulkan sdl2)

add_dependencies(big_satellite_lab Shaders)