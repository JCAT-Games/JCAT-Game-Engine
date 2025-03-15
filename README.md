# JCAT-Game-Engine

[![Discord](https://img.shields.io/discord/1281033293441269974.svg?label=JCAT%20Game%20Engine%20Discord&logo=discord&color=7289DA)](https://discord.gg/eJx6G9yygP)

JCAT Game engine is a project where we build a game engine from scratch using
Vulkan and C/C++. We started this project because we wanted to learn how low level graphics
programming works and wanted to learn the process of how a game is created from the
primitive level. The goal by the end of this semester is to have a functional game engine that we
could use to develop a game with. We also want to be able to have a deep understanding of
how the game engine works.

## Project Setup - Windows
To edit and run the project on a Windows computer, follow these steps:
1. (Optional) Install GitHub Desktop
   - This isn't necessary, but may be useful to work with the repository on your local device
2. Clone this GitHub Repository to your local device
3. Download [CMake](https://cmake.org/download/) (Any recent version is fine)
   - Scroll down to "Binary Distributions" and download the .msi file that corresponds to your Windows platform (see left column)
   - Follow any instructions provided by the installer
4. Install the Vulkan SDK version 1.3.290 (under Windows listed as "SDK Installer")
   - Follow any instructions provided by the SDK InstallerW

![MinGW_DownloadImage](https://github.com/user-attachments/assets/b732da29-5df2-4d9d-9a6b-26de9c9b50ce)

5. Download the the highlighted .zip file in the above image from [here](https://winlibs.com/)
   - Extract the folder within the .zip file (named mingw64) and put it in a known location
6. Download "glm-1.0.1-light.zip" from [here](https://github.com/g-truc/glm/releases)
   - Extract the folder within the .zip file (named glm) and put it in a known location
7. Click the "64-bit Windows binaries" button at [this link](https://www.glfw.org/download) to download the GLFW .zip file
   - Extract the folder within the .zip file (named glfw-3.4.bin.WIN64) and put it in a known location
8. Download the stb_image.h file located [here](https://github.com/nothings/stb/blob/master/stb_image.h) and store it in a known location
9. Go to the where you cloned the GitHub Repository to and make a copy of the envWindowsTemplate.cmake file named ".env.cmake"
10. Change the file paths in this .env.cmake file to be the locations of the previously downloaded files and folders
    - Follow the instructions in the comments and the example file paths
11. Find "Edit the system environment variables" by typing "environment" into your Windows search bar.
12. In the window that pops up, click the button that says "Environment Variables" at the bottom
13. In the new pop-up window, click "Path" and then the "Edit" button
14. Finally, click "New" and then paste the file path to your mingw folder's bin folder (".../mingw/bin")
15. To run the game engine application, go to the folder where you cloned the repository, select mingwBuild (a .bat file) and when that finishes, go to the "build" file select the application file named "JCATEngine.exe"

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
