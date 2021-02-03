# big-satellite-lab

![](assets/readme_pics/logo_transparent.png?raw=true "logo")

Big Satellite Lab is a club at the University of Georgia where we aim to create a real-time 3D simulation of our solar system as a sandbox for testing spacecrafts.

## Building the project

Note: some of the instructions for building the project are borrowed from https://vkguide.dev/docs/chapter-0/building_project/

First, install the Vulkan SDK https://vulkan.lunarg.com/sdk/home. This will contain the development headers and libraries needed to create Vulkan applications. It is put in a global location which we will reference later with CMake.

Now download CMake, as it’s the build system we are going to use. https://cmake.org/.

Once CMake is installed, open CMake-gui. In the "Where is the source code" path, navigate to the root of the project containing the CMakeLists.txt file. This will be the folder called `big-satellite-lab`. For the "Where to build the binaries" path, select the build folder `big-satellite-lab/build`.

![](assets/readme_pics/cmake.png?raw=true "cmake")

In the Vulkan drop down you may find that the variables need to be populated with the Vulkan loader, include, and glslc directories. This will be wherever you installed the Vulkan SDK.

Now press configure. Select the IDE that you want to use (I use Visual Studio).

At this point, it should give you an error about missing SDL2.

SDL2 is a library dependency that we need to get separately from the rest.

You can grab it here https://www.libsdl.org/download-2.0.php . I recommend you just take the development libraries. Once you have unzipped SDL somewhere, put the root path in the `SDL2_DIR` variable of CMake. Note, this is not the /libs or similar folder, but the root folder of SDL itself. 

Press generate. If you open `big-satellite-lab/build` you will see the project files. If you're using Visual Studio, open the file called `big_satellite_lab.sln`.

From Visual Studio, select the big-satellite-lab target, set as Startup Project, and you can now compile and execute it by just hitting F5 (start debugging).

![](assets/readme_pics/startup.png?raw=true "startup")

If you get the message of SDL2 dll missing, go to your sdl folder, lib directory, and grab the dlls from there. Paste them on big-satellite-lab/bin/Debug/ folder (or Release) . The dll has to be in the same directory as vulkan_guide.exe.

![](assets/readme_pics/debug_bin.png?raw=true "debug_bin")

Now you should be able to build and run the program, and should be greated by a 3D scene in space.

## Project organization

You may notice that there are many C++ files in the visual studio project. Most of these are from the [Vulkan renderer](https://github.com/David-DiGioia/vulkan-renderer), which is a submodule of this project. You are welcome to contribute to the renderer, but a bulk of the work for Big Sat Lab will be done in this repo.

At the time of writing this tutorial, there are only three C++ files in this repo: `big_sat_lab.cpp`, `big_sat_lab.h`, and `main.cpp`. All the exciting stuff happens in `big_sat_lab.cpp` so open that file.

You will notice four important functions: `init`, `update`, `input`, and `gui`. Let's explore each of these.

### init

`init` is called once at program startup, so this is a good place to initialize the scene. This could include placing 3D objects, lights, and the camera. Let's take a look an example:

```
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

	Light light{};
	light.color = glm::vec4{ 1.0, 0.8, 1.0, 100.0 }; // w component is intensity
	light.position = glm::vec4{ 1.0, 8.0, 4.0, 0.0 }; // w component is ignored
	// Only lights in this vector will be rendered!
	_lights.push_back(light);
}
```

Notice that it takes a `VulkanEngine` object as an argument. `VulkanEngine` is the object which does all the rendering, which means if we want anything to show up on the screen, this object will be the thing that makes that happen.

GLM is a linear algebra library that we will be using a lot. The most useful objects are `vec3`s which is just a vector of 3 floats, and `mat4`s which are 4x4 matrices of floats.

```
_camera.pos = glm::vec3{ 0.0, 7.0, 7.0 };
```

This simply sets the camera's position to (0, 7, 7).

```
_earth.setRenderObject(engine.create_render_object("earth"));
_earth.setPos(glm::vec3(-2.5, 0.0, 0.4));
_earth.setRot(glm::rotate(glm::radians(110.0f), glm::vec3{ 0.0, 1.0, 0.0 }));
```

`GameObjects` are any 3D object in our scene. They consist of just a transform (position, rotation, scale) and a `RenderObject*` which is an object that `VulkanEngine` knows how to draw to the screen. Here, `_earth` is a `GameObject` and we are assigning a `RenderObject` to it so that it can be drawn to the screen, then we are setting its position and rotation.

`_earth` is a member variable of BigSatLab, which we append with an underscore to make it easily distinguishable from local variables. Feel free to add member variables and functions as you need to in `big_sat_lab.h`.

Note that the planet models are 1 : 1,000,000 their true size in meters. So if we were to scale `_earth` by a million, it would have its true size if we treat 1 unit as 1 meter.

The `create_render_object` function takes a string as a parameter, and this string corresponds to the name of a folder in the `assets/models` directory. We can also pass a second argument to specify the name of the material we want to use, but by default it will assume the material's name is also "earth". The name of the material corresponds to an entry in `shaders/_load_materials.txt` but you won't have a need to make any changes there unless you create a new material.

```
Light light{};
light.color = glm::vec4{ 1.0, 0.8, 1.0, 100.0 }; // w component is intensity
light.position = glm::vec4{ 1.0, 8.0, 4.0, 0.0 }; // w component is ignored
// Only lights in this vector will be rendered!
_lights.push_back(light);
```

Lastly, we add a point light to the scene. The `color` member determines both the color and intensity of the light. The first three components determine the color of the light and the fourth component (also called w) determines intensity. The color components represent the amount of red, green, and blue respectively and each are in the range [0, 1]. So in this example, the light has 1.0 red, 0.8 green, 1.0 blue and an intensity of 100.

For the light to actually show up in the scene we must add it the the `_lights` vector.

### update

`update` is called once every frame, so we can do fun things like make planets move around. Here's an example:

```
// This is called once per frame
void BigSatLab::update(VulkanEngine& engine, float delta)
{
	glm::vec3 pos{ _earth.getPos() };
	pos.y = std::sinf(_time);
	_earth.setPos(pos);

	updateCamera(engine);
	engine.set_scene_lights(_lights);
	_time += delta;
}
```

This function has an additional paramter, `delta`. This is the number of seconds it took to render the last frame. This is helpful because it allows us to make the speed of things depend only on time and not on framerate.

`_time` is the elapsed time since the start of the program. Notice we get this by just adding up the times of each individual frame. We set the y position of `_earth` to be sin(\_time) so it should oscillate up and down with a period of 2\*pi seconds. If you run the program, you'll see that's exactly what happens.

Lastly we make upload the changes we made to the camera and lights to the `VulkanEngine` object so that they're actually image we're rendering is up to date.

### input

`input` is for handling usesr input, such as keyboard presses and mouse movement. Here is an example:

```
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
```

SDL is the window library we're using, and it's also reponsible for knowing when any buttons are pressed. `SDL_GetKeyboardState` gives us the state of the whole keyboard so we can know exactly which keys are being pressed. In this example, we change the position of the camera based on the WASD keys.

We also check for mouse motion to change the rotation of the camera, but only if `_camMouseControls` is enabled. This can be toggled by pressing F when the program is running.

Notice that for checking if the user presses F, we use an `SDL_Event` but for checking if WASD are pressed we use keystates. `SDL_Event`s are better for discrete key presses, like toggling something, whereas keystates are better for continuous keypresses, like holding W to move forward.

If `init` returns `true` the program will exit.

### gui

GUI stands for graphical user interface. It includes health bars, menus, etc. It's also a great tool for debugging because it allows us to change parameters while the application is running. The project is already setup to run with the Dear ImGui library.

```
// This is called once per frame to update GUI
void BigSatLab::gui()
{
	ImGui::ShowDemoWindow();
}
```

Right now the we're just displaying ImGui's demo window, which is one of the best resources for learning what everything does in ImGui. You can find its implementation in `imgui_demo.cpp` (in the imgui project).

![](assets/readme_pics/imgui_demo.png?raw=true "imgui_demo")

A fast way to learn ImGui is to just run the program with the demo window, and look through each of the gizmos it has to offer, and if you want to use that gizmo for your own window, look in the `imgui_demo.cpp` and find the relevant code and copy and paste it for your own use.

### The examples put together

If you run the program with each of these implementations for `init`, `update`, `input`, and `gui` you will see the following:

![](assets/readme_pics/demo.png?raw=true "demo")

Earth oscillates up and down and we can move the camera around with WASD. If you press F, that will enable/disable looking around with the camera. Also notice the ImGui window which you can click to expand tabs of. We can associate variables from out program with these sliders/fields and then we can change their value from the gui.

Hopefully this gives you a good starting point to understand how the project is organized so you can start making contributions and bringing the program's behavior closer to that of our solar system.
