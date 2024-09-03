# Atlas

![Atlas](/Resources/Branding/AtlasLogo.png?raw=true "Atlas")

The Atlas Engine is an experimental project, which was initially built following [The Cherno's](https://www.youtube.com/@TheCherno) Game Engine series.

## Getting Started
At the moment, Visual Studio 2022 is recommended. Atlas is officially untested on other development environments whilst I focus on a Windows build.
Here are the following steps to get Atlas running on your computer:
1. Clone the repository to a local destination using git: `git clone --recursive https://github.com/Bart-soldier/Atlas`,
2. Execute `scripts/Win-GenProjects.bat`,
3. Launch `Atlas.sln`
4. Build and execute the project.

When cloning the repository, make sure that you do a `--recursive` clone to fetch all of the submodules!

## External dependencies
This project uses the following external dependencies:
- **Windows:** [glfw](https://github.com/Bart-soldier/glfw) (forked),
- **OpenGL:** [Glad](https://glad.dav1d.de/),
- **Logging:** [spdlog](https://github.com/gabime/spdlog),
- **UI:** [imgui](https://github.com/Bart-soldier/imgui) (forked from 'Docking'),
- **Texture Loading:** [stb_image](https://github.com/nothings/stb/tree/master),
- **Math:** [glm](https://github.com/g-truc/glm),
- **ECS:** [entt](https://github.com/skypjack/entt/tree/master).