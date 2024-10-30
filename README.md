# Atlas

![Atlas](/Resources/Branding/AtlasLogoName.png?raw=true "Atlas")

## Preamble

The Atlas Engine is an experimental project. Its purpose is to act as a sandbox for studying, implementing and demonstrating different kinds of rendering techniques and systems, not as a fully-featured infallible engine.
Furthermore, the output of some shader programs depend on the graphics device. It is recommended to run the Atlas Editor with an NVIDIA graphics card, analogously to the development environment. Please read [SELECTING_NVIDIA.md](https://github.com/Bart-soldier/Atlas/blob/main/SELECTING_NVIDIA.md) for instructions on how to selected your NVIDIA card as the editor's graphics environment.

## Getting Started
At the moment, the Atlas Engine as only been tested on a Windows x64 system with Visual Studio 2022. Atlas is officially untested on other development environments. Thereafter are the steps to get the Atlas Engine running on your computer.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/Bart-soldier/Atlas`.

When cloning the repository, make sure that you do a `--recursive` clone to fetch all of the submodules. If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

1. Run the [Setup.bat](https://github.com/Bart-soldier/Atlas/blob/master/scripts/Setup.bat) script file found in the `scripts` folder. This script will help you download the required prerequisites for the project.\
**N.B.:** Python is required to be able to run the script. Also, you may be required to run the script multiple times.
2. As part of the script, if they are not yet installed, you will be prompted to download a Python package, the Premake executable, and the Vulkan SDK (`[Y]` when asked).\
**N.B.:** On the Vulkan SDK installer, make sure to also install the "Shader Toolchain Debug Symbols" to be able to run the project in Debug mode.
3. Once the installation process is completed, Premake will generate a Visual Studio solution file. Launch `Atlas.sln` from the [root folder](https://github.com/Bart-soldier/Atlas/blob/main/) to open the solution.
4. You can now build and execute the project!

If changes are made, or if you want to regenerate project files, rerun the [Win-GenProjects.bat](https://github.com/Bart-soldier/Atlas/blob/main/scripts/Win-GenProjects.bat) script file found in `scripts` folder, as this script only reruns the Premake files.

## Known issue
Not all shader programs properly link when using an Intel graphics device, as opposed to NVIDIA.

## External dependencies
This project uses the following external dependencies:
- **Project setup:** [Premake](https://premake.github.io/) (.exe),
- **Windows:** [glfw](https://github.com/Bart-soldier/glfw) (forked),
- **OpenGL:** [Glad](https://glad.dav1d.de/),
- **OpenGL Math:** [glm](https://github.com/g-truc/glm),
- **Vulkan:** [SDK](https://vulkan.lunarg.com/sdk/home),
- **Logging:** [spdlog](https://github.com/gabime/spdlog),
- **UI:** [imgui](https://github.com/Bart-soldier/imgui) (forked from 'Docking'),
- **UI Gizmo:** [ImGuizmo](https://github.com/Bart-soldier/ImGuizmo) (forked),
- **Texture Loading:** [stb_image](https://github.com/nothings/stb/tree/master),
- **ECS:** [entt](https://github.com/skypjack/entt/tree/master),
- **Serialization:** [yaml](https://github.com/Bart-soldier/yaml-cpp) (forked),
- **Shader compiling:** [shaderc](https://github.com/google/shaderc) and [SPIR-V](https://www.khronos.org/spir/),
- **Model loading:** [assimp](https://github.com/Bart-soldier/assimp) (forked).


## Resources used
This project used the following resources:
- [The Cherno's](https://www.youtube.com/@TheCherno) Game Engine series,
- [LearnOpenGL](https://learnopengl.com/).
