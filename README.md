# Atlas

![Atlas](/Resources/Branding/AtlasLogoName.png?raw=true "Atlas")

The Atlas Engine is an experimental project.

## Getting Started
At the moment, Visual Studio 2022 is recommended. Atlas is officially untested on other development environments. Thereafter are the steps to get the Atlas Engine running on your computer.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/Bart-soldier/Atlas`.

When cloning the repository, make sure that you do a `--recursive` clone to fetch all of the submodules. If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

1. Execute [Win-GenProjects.bat](https://github.com/Bart-soldier/Atlas/blob/main/scripts/Win-GenProjects.bat) script file found in the `scripts` folder,
2. Launch `Atlas.sln` from the [root folder](https://github.com/Bart-soldier/Atlas/blob/main/),
3. Build and execute the project.

If changes are made, or if you want to regenerate project files, rerun the [Win-GenProjects.bat](https://github.com/Bart-soldier/Atlas/blob/main/scripts/Win-GenProjects.bat) script file found in `scripts` folder.

## External dependencies
This project uses the following external dependencies:
- **Windows:** [glfw](https://github.com/Bart-soldier/glfw) (forked),
- **OpenGL:** [Glad](https://glad.dav1d.de/),
- **Logging:** [spdlog](https://github.com/gabime/spdlog),
- **UI:** [imgui](https://github.com/Bart-soldier/imgui) (forked from 'Docking'),
- **Texture Loading:** [stb_image](https://github.com/nothings/stb/tree/master),
- **Math:** [glm](https://github.com/g-truc/glm),
- **ECS:** [entt](https://github.com/skypjack/entt/tree/master),
- **Seriaization:** [yaml](https://github.com/Bart-soldier/yaml-cpp).

## Resources used
This project used the following resources:
- [The Cherno's](https://www.youtube.com/@TheCherno) Game Engine series.
