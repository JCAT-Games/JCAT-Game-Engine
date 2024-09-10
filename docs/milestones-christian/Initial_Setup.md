# Creating a Vulkan Game Engine - Tutorials 1 - 4:

To help create a Vulkan game engine, this [youtube playlist](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR) was used.

This file contains the instructions for setting up the main application class, the game window, and the vulkan device.

## Step 0: Setup and Importing libraries:

Instructions for setting up the development environment for the game engine:

### IDE and software:
To create our game engine, we will be using C/C++ with Visual Studio Code.

### External Libraries:
1. Vulkan version 1.3.290.0
2. GLFW 3.4
3. GLM

These are imported into the project using a makefile.

## Step 1: Setting up the main application class:

Instructions for setting up the main application class:

### main.cpp:

The main file should create an instance of the application class and run the application.

### app.hpp:

Contains the main application class. The main application class should consist of:
1. The fixed default resolutions for the width and height of the screen.
2. The function for running the application
3. The private variables for the every setup process of the game engine which includes but is not limited to:
    * The game window
    * The devices used in the game engine
    * The graphics pipeline

### app.cpp:

This file should contain the logic for any functions longer than one line that are in the main application class. This includes determining whether the window should close or not at any given moment (basically creating the game loop).

## Step 2: Setting up the game window with GLFW:

To set up the game window, we will need a seperate class for it. This will require the creation of two new files:

### window.hpp:

Contains the window class for the game engine. Should include functions such as:
1. Initializig GLFW
2. Creating the game window with the given properties
3. Creating the window surface
4. Closing the window

### window.cpp:

Contains the logic for each of the window functions.
The process to create a window includes:
1. Initializing GLFW
2. Determining whether the window will use an API or not
3. Deterining whether the window is resizable or not
4. Calling the glfwCreateWindow function with a given width, height, and title to create a new GLFW window.

## Step 3: Setting up the device:

We will need a seperate class to set up the device which will be split up across the following files:

### device.hpp:

Contains the class used to set up the device for vulkan.
Here is the funcions for setting up the devices:
1. Setup the frame of the class, including:
    * Constructor - calls all the other functions for creatig the device
    * Destructor - destroys the current device from memory and the current Vulkan instance
    * Make an instance of this class not copyable or moveable
    * Creating the command pool
    * Creating the Vulkan Surface 
2. Create the Vulkan instance:
    * Declare the API version
    * Extract the required extensions
    * Initialize the debug utilities
    * initialize the Vulkan instance based on this information
3. Pick the physical device:
    * See how many devices the system has
    * Check if the device is suitable by ensuring that it support swap chain
    * Extract and implement the device properties
4. Create the logical device:
    * create an interface to interact with the physical device
    * enable device extensions and features necessary
5. Create the command pool:
    * used to create command buffers for the GPU
    * record and submit drawing commands and other operations to the GPU
6. Surface and Suitability:
    * Create the Vulkan surface on the GLFW window
    * check to ensure that the physical device support the necessary features
7. Utility functions
    * Creating an image buffer
    * hceck for a supporting image format