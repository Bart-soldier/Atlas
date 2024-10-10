-- Atlas Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Atlas/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Atlas/vendor/Glad/include"
IncludeDir["imgui"] = "%{wks.location}/Atlas/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Atlas/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Atlas/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Atlas/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Atlas/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Atlas/vendor/yaml-cpp/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Atlas/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["assimp"] = "%{wks.location}/Atlas/vendor/assimp/include"
IncludeDir["assimp_config"] = "%{wks.location}/Atlas/vendor/assimp/_config_headers"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"