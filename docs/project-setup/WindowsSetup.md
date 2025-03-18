# Project Setup - Windows

To edit and run the project on a Windows computer, follow these steps:
1. (Optional) Install GitHub Desktop
   - This isn't required, will likely be useful to work with the GitHub repository on your local device
2. Clone this GitHub Repository to your local device

<img src="https://github.com/user-attachments/assets/9d34269d-1c8b-41c3-8cec-aa6a9689df3c" width="700">

3. Download [CMake](https://cmake.org/download/) (Any recent version is fine)
   - Scroll down to "Binary Distributions" and download the .msi file that corresponds to your Windows platform (see left column)
   - Follow any instructions provided by the installer

<img src="https://github.com/user-attachments/assets/aebd4e93-685b-4f94-beec-6fcf263f6815" width="700">

4. Install the Vulkan SDK version 1.3.290 [here](https://vulkan.lunarg.com/sdk/home) (under Windows listed as "SDK Installer")
   - Follow any instructions provided by the SDK Installer

<img src="https://github.com/user-attachments/assets/b732da29-5df2-4d9d-9a6b-26de9c9b50ce" width="700">

5. Download the the highlighted .zip file in the above image from [here](https://winlibs.com/)
   - Extract the folder within the .zip file (named mingw64) and put it in a known location
6. Download "glm-1.0.1-light.zip" from [here](https://github.com/g-truc/glm/releases) (scroll down to find file)
   - Extract the folder within the .zip file (named glm) and put it in a known location
7. Click the "64-bit Windows binaries" button at [this link](https://www.glfw.org/download) to download the GLFW .zip file
   - Extract the folder within the .zip file (named glfw-3.4.bin.WIN64) and put it in a known location
8. Download the stb_image.h file located [here](https://github.com/nothings/stb/blob/master/stb_image.h) and store it in a known location
9. Download the tiny_obj_loader.h file located [here](https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h) and store it in a known location
10. Go to the folder where you cloned the GitHub Repository to (perhaps named JCAT Game Engine) and make a copy of the envWindowsTemplate.cmake file and name it ".env.cmake" (in Visual Studio Code, the file icon may change to a green dollar sign)
11. Change the file paths in this .env.cmake file to be the locations of the previously downloaded files and folders
    - Follow the instructions in the comments and the example file paths ("Path/To/Your...")
12. Add the mingw bin folder to your Path Environment Variable
    - Find "Edit the system environment variables" by searching for it in your Windows search bar.
    - In the window that pops up, click the button that says "Environment Variables" at the bottom
    - In the new pop-up window, click "Path" and then the "Edit" button
    - Finally, click "New" and then paste the file path to your mingw folder's bin folder (".../mingw/bin")
13. To run the game engine application, go to the folder where you cloned the repository , select mingwBuild (a .bat file) and when that finishes running, go to the "build" folder and finally select the application file named "JCATEngine.exe"
    - The application is running correctly if a new window named "JCAT Game Engine" shows a non-blank screen