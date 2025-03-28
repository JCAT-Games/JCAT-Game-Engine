# I used the knowledge from this YouTube Tutorial to create this CMakefile:
# https://www.youtube.com/watch?v=ZuHK_5cJ6B8&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=28 

# Include the .env.cmake file, which ideally should exist in order to compile this program
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

# Minium CMake version is 3.11.0
cmake_minimum_required(VERSION 3.11.0)

# Set the name of the compiled binary file to JCATengine
set(NAME JCATengine)
project(${NAME} VERSION 0.0.0.1)

# Check if MinGW makefiles is the generator being used:
message(STATUS "using ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    # Must have the MINGW_PATH defined in the .env.cmake file
    if (NOT MINGW_PATH)
        message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    # Will use MINGW compiler
    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)
endif()

# LOCATING VULKAN:
# VULKAN_SDK_PATH Should be defined in the .env.cmake file
# If it is, it will use the given path
if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # Make sure this path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # Make sure this path is correct
    set(Vulkan_FOUND "True")
# Manually attempt to find the Vulkan Package
else()
    # Vulkan_FOUND is automatically set here
    find_package(Vulkan REQUIRED) # Throws an error if the package is not found
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()

# Check to make sure a Vulkan SDK was successfully located before proceeding
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find the Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# Now we must locate where the GLFW library is
# GLFW_PATH should be defined in the .env.cmake file
# If it is, it will use the path given
if (DEFINED GLFW_PATH)
    message(STATUS "Using GLFW path specified in .env.cmake")
    set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include") # Make sure this file path is correct
    # If the project is using Visual Studio
    if (MSVC)
        set(GLFW_LIB "${GLFW_PATH}/lib-vc2019") # Use the same version as your Visual Studio (ex: 2019)
    # If the project is using mingw
    elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
        message(STATUS "USING MINGW Makefiles")
        set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # Make sure this file path leads to the mingw library if using mingw
    endif()
# If a path in the .env.cmake is not defined, it will manually try to find it
else()
    find_package(glfw3 3.3 REQUIRED)
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()

# Check to make sure the GLFW library was found
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

# Include external folder (If needed for other external libraries)
include_directories(external)

if (DEFINED STB_PATH)
    message(STATUS "Using STB_PATH from .env.cmake: ${STB_PATH}")
    set(STB_IMAGE_INCLUDE_DIR "${STB_PATH}")
    
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE ${STB_IMAGE_INCLUDE_DIR})
else()
    message(FATAL_ERROR "STB_PATH is not defined in .env.cmake!")
endif()

if (DEFINED TINY_OBJ_PATH)
    message(STATUS "Using TINY_OBJ_PATH frm .env.cmake: ${TINY_OBJ_PATH}")
    set(TINY_OBJ_INCLUDE_DIR "${TINY_OBJ_PATH}")

    add_library(tiny_obj INTERFACE)
    target_include_directories(tiny_obj INTERFACE ${TINY_OBJ_INCLUDE_DIR})
else()
    message(FATAL_ERROR "TINY_OBJ_PATH is not defined in .env.cmake!")
endif()

# Gathers all .cpp files in the source directory and all subdirectories and compiles hem to an executable using C++ 17
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/source/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# If user is on Windows
if (WIN32)
    # Create a build for Windows
    message(STATUS "CREATING BUILD FOR WINDOWS")
 
    # Check to see if the user has configured MINGW
    # If the user has, use this compiler to build the project
    if (USE_MINGW)
        target_include_directories(${PROJECT_NAME} PUBLIC
            ${MINGW_PATH}/include
        )
        target_link_directories(${PROJECT_NAME} PUBLIC
            ${MINGW_PATH}/lib
        )
    endif()
 
    # Creates the include path for compilation, linking all external library directories and the source directory
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/source
        ${Vulkan_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
        ${GLM_PATH}
        ${STB_IMAGE_INCLUDE_DIR}
        ${TINY_OBJ_INCLUDE_DIR}
    )
 
    # Setting Up Linker Directories for compilation
    target_link_directories(${PROJECT_NAME} PUBLIC
        ${Vulkan_LIBRARIES}
        ${GLFW_LIB}
    )
 
    # Linking required libraries
    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1 stb_image tiny_obj)
# If use is on Linux
elseif (UNIX)
    # Create a build for Linux
    message(STATUS "CREATING BUILD FOR UNIX")

    # Set include directories for Linux
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/source
    )

    # Linking required libraries
    target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES} stb_image tiny_obj)
endif()

##### For Compiling Shader Objects #####
# Credit: https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt

# First we must find the GLSL Validator glslc
find_program(GLSL_VALIDATOR glslangValidator HINTS
    ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
    /usr/bin 
    /usr/local/bin 
    ${VULKAN_SDK_PATH}/Bin
    ${VULKAN_SDK_PATH}/Bin32
    $ENV{VULKAN_SDK}/Bin/ 
    $ENV{VULKAN_SDK}/Bin32/
)

# Check if the GLSL validator was found
if(NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "GLSL Validator not found! Please install it or set the VULKAN_SDK_PATH.")
else()
    message(STATUS "GLSL Validator found at: ${GLSL_VALIDATOR}")
endif()

# Now we must take in all .vert and .frag shader files.
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

# Output the list of found shader files for verification
message(STATUS "Found GLSL source files: ${GLSL_SOURCE_FILES}")

# Now we must compile the shader files into SPIR-V files
foreach(GLSL_FILE ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL_FILE} NAME)
    set(SPIRV_FILE "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL_FILE} -o ${SPIRV_FILE}
        DEPENDS ${GLSL_FILE}
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV_FILE})
endforeach(GLSL_FILE)

# Run the shaders compilation with the "Shaders" keyword
add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
