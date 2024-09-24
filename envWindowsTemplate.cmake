# Use the template to create your .env.cmake file, which you will use to find the dependencies needed for this project on your specific device.
# This is for users on Windows

# replace the paths with the actual path to your external libraries
set(GLFW_PATH "Path/To/Your/GLFW/Version/Folder/For/Example/glfw-3.4.bin.WIN64")
set(GLM_PATH "Path/To/Your/GLM/Folder/Named/glm")
set(VULKAN_SDK_PATH "Path/To/Your/Vulkan/Version/Folder/For/Example/1.3.290.0")

# Set this path if you are NOT using Visual Studio
set(MINGW_PATH "Path/To/Your/mingw/Folder/Named/mingw64")