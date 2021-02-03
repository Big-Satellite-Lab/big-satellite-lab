# big-satellite-lab

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

![](assets/readme_pics/startup.png?raw=true "cmake")

If you get the message of SDL2 dll missing, go to your sdl folder, lib directory, and grab the dlls from there. Paste them on big-satellite-lab/bin/Debug/ folder (or Release) . The dll has to be in the same directory as vulkan_guide.exe.

![](assets/readme_pics/debug_bin.png?raw=true "cmake")

Now you should be able to build and run the program, and should be greated by a 3D scene in space.

## Cool now what

