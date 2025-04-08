# Project Setup - macOS

To edit and run the project on a computer using macOS, follow these steps:
1. (Optional) Install GitHub Desktop
   - This isn't required, will likely be useful to work with the GitHub repository on your local device
2. Clone this GitHub Repository to your local device

<img src="https://github.com/user-attachments/assets/65b36f82-3c50-40ab-8532-2251f6f21b74" width="640">

3. Download [CMake](https://cmake.org/download/) (Any recent version is fine)
   - Scroll down to "Binary Distributions" and download the .dmg file that corresponds to your Mac version (see left column)
   - Follow any instructions provided by the installer

<img src="https://github.com/user-attachments/assets/097695b3-ae10-4157-bce8-864dc3269c44" width="640">

4. Install the Vulkan SDK version **1.3.290** [here](https://vulkan.lunarg.com/sdk/home) (under Mac listed as "SDK Installer")
   - Follow any instructions provided by the SDK Installer
5. Download "glm-1.0.1-light.zip" from [here](https://github.com/g-truc/glm/releases) (scroll down to find file)
   - Extract the directory within the .zip file (named glm) and put it in a known location
6. Click the "64-bit macOS binaries" button at [this link](https://www.glfw.org/download) to download the GLFW .zip file
   - Extract the directory within the .zip file (named glfw-3.4.bin.WIN64) and put it in a known location
7. Download the stb_image.h file located [here](https://github.com/nothings/stb/blob/master/stb_image.h) and store it in a known location
8. Download the tiny_obj_loader.h file located [here](https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h) and store it in a known location
9. Go to the directory where you cloned the GitHub Repository to (perhaps named JCAT Game Engine) and make a copy of the envUnixTemplate.cmake file and name it ".env.cmake" (in Visual Studio Code, the file icon may change to a green dollar sign)
10. Change the file paths in this .env.cmake file to be the locations of the previously downloaded files and directories
    - Follow the instructions in the comments and the example file paths ("/path/to/your...")
11. To run the game engine application, go to the directory where you cloned the repository, select unixBuild (a .sh file) and when that finishes running, go to the "build" directory and finally select the application file named "JCATEngine.exe"
    - The application is running correctly if a new window named "JCAT Game Engine" shows a non-blank screen
    - If your computer has an Nvidia GPU and either is plugged in or has no other GPU, the game engine may 
    crash on start up. If this happens, keep running the application and it should eventually work.