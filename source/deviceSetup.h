#ifndef DEVICE_SETUP_H
#define DEVICE_SETUP_H

/**
 * @file deviceSetup.h
 * @brief Header file for setting up the Vulkan device and dependencies.
 *
 * This file contains the declarations for the DeviceSetup class and 
 * related structs for handling Vulkan's logical device, queue families, 
 * and swap chain support details. 
 */

#include "window.h"

// std namespace
#include <cstdint>
#include <vector>

/** Used for checking if the current device is plugged in or not (Imports based on operating system) */
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__)
    #include <fstream>
    #include <string>
#endif

/**
 * @brief JCAT game engine namespace
 *
 * Includes all the logic for the JCAT Game Engine.
 */
namespace JCAT {
    /**
     * @brief Holds the indices of the queue families needed for rendering and presenting.
     *
     * This struct stores the indices of the graphics and present queue families 
     * required for rendering operations and presenting images to the swap chain. 
     * It also includes flags to indicate if these families have valid indices.
     */
    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;

        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;

        /**
         * @brief Checks if both graphics and present family indices are set.
         * @return True if both indices are valid, false otherwise.
         */
        bool isComplete() {
            return graphicsFamilyHasValue && presentFamilyHasValue;
        }
    };

    /**
     * @brief Contains the support details for the swap chain.
     *
     * This struct encapsulates the details required to create a swap chain,
     * including the capabilities of the surface, available formats, 
     * and presentation modes.
     */
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    /**
     * @brief Represents a Vulkan device setup and management class.
     *
     * This class encapsulates the setup and management of Vulkan logical and physical devices, 
     * as well as debug utilities. It provides functionality to initialize the Vulkan instance,
     * create a logical device, set up a debug messenger, and query queue families.
     *
     * @details The class is responsible for selecting an appropriate physical device (GPU),
     * configuring the required queue families, and handling logical device creation. It also
     * provides error handling mechanisms using exceptions.
     */
    class DeviceSetup {
        public:
            /** ### Constructor & Destructor ### */

            /**
             * @brief Constructs a DeviceSetup instance for Vulkan device management.
             *
             * @param window Reference to the window for creating a Vulkan surface.
             */
            DeviceSetup(Window& window);

            /**
             * @brief Destroys the DeviceSetup instance and releases associated resources.
             */
            ~DeviceSetup();


            /** ### Delete Copy, Move, Assignment, and Move Assignment operators ### */
            DeviceSetup(const DeviceSetup&) = delete;
            void operator=(const DeviceSetup&) = delete;
            DeviceSetup(DeviceSetup&&) = delete;
            DeviceSetup& operator=(DeviceSetup&&) = delete;


            /** ### Accessor Functions */


            /**
             * @brief Retrieves the command pool handle.
             *
             * @return VkCommandPool The command pool handle used for allocating command buffers.
             */
            VkCommandPool getCommandPool();

            /**
             * @brief Retrieves the logical device handle.
             *
             * @return VkDevice The logical device handle for interacting with the GPU.
             */
            VkDevice device();

            /**
             * @brief Retrieves the Vulkan surface handle for the window.
             *
             * @return VkSurfaceKHR The Vulkan surface handle created for the window.
             */
            VkSurfaceKHR windowSurface();

            /**
             * @brief Retrieves the graphics queue handle.
             *
             * @return VkQueue The graphics queue handle for submitting graphics commands.
             */
            VkQueue graphicsQueue();

            /**
             * @brief Retrieves the present queue handle.
             *
             * @return VkQueue The present queue handle for presenting images to the window.
             */
            VkQueue presentQueue();

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            VkPhysicalDeviceProperties properties;
        private:
            #ifndef NDEBUG
                // Disable validation layers in release mode for performance.
                const bool enableValidationLayers = false;
            #else
                // Enable validation layers in debug mode to assist with debugging Vulkan applications.
                const bool enableValidationLayers = true;
            #endif


            /* ### Main Functions ### */


            /**
             * @brief Creates a Vulkan instance.
             *
             * This function initializes the Vulkan instance, which is the entry point into Vulkan. 
             * The instance serves as a connection between the application and the Vulkan library.
             *
             * @throws std::runtime_error If the Vulkan instance creation fails.
             */
            void createVulkanInstance();

            /**
             * @brief Creates a window surface.
             *
             * This function creates a Vulkan surface that will be used for rendering. 
             * It connects the windowing system (e.g., GLFW) with Vulkan, enabling graphics to be drawn to a window.
             *
             * @throws std::runtime_error If the window surface creation fails.
             */
            void createWindowSurface();

            /**
             * @brief Selects a suitable physical device (GPU).
             *
             * This function picks a physical device (such as a GPU) that supports the required Vulkan features.
             * It checks device properties, queue families, and extension support to ensure compatibility with the application.
             * It also checks whether to use integrated or discrete GPU if they exist.
             *
             * @throws std::runtime_error If no suitable physical device is found.
             */
            void pickPhysicalDevice();

            /**
             * @brief Creates a logical device for interfacing with the physical device.
             *
             * This function creates a logical device, which allows the application to interact with the chosen physical device (GPU).
             * It also selects specific features and queue families (e.g., graphics and presentation) for use in rendering.
             *
             * @throws std::runtime_error If logical device creation fails.
             */
            void createLogicalDevice();

            /**
             * @brief Creates a command pool for allocating command buffers.
             *
             * This function creates a command pool associated with the graphics queue
             * family index. The command pool can be used to allocate command buffers,
             * which in turn can be used to record and submit commands to the GPU.
             *
             * @return None, but sets the commandPool instance variable in the DeviceSetup class.
             * @throws std::runtime_error If the command pool creation fails.
             */
            void createCommandPool();

            /**
             * @brief Sets up the Vulkan debug messenger.
             *
             * This function configures the Vulkan debug messenger to capture and report validation layers' 
             * debug messages, including errors, warnings, and performance issues. The debug messenger helps 
             * identify issues during development by logging diagnostic messages.
             *
             * @throws std::runtime_error If setting up the debug messenger fails.
             */
            void setupDebugMessenger();


            /* ### Helper functions for main functions ### */


            bool isDeviceSuitable(VkPhysicalDevice device);
            std::vector<const char*> getRequiredGLFWExtensions();
            bool checkValidationLayerSupport();
            void hasGLFWRequiredInstanceExtensions();
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            bool checkDeviceExtensionSupport(VkPhysicalDevice device);
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
            bool isOnBatteryPower();

            /** ### Debug Messnger Functions ### */

            VkResult createDebugUtilsMessenger(VkInstance instance, 
                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                                const VkAllocationCallbacks* pAllocator, 
                                                VkDebugUtilsMessengerEXT* pDebugMessenger);
            void destroyDebugUtilsMessengerEXT(VkInstance instance, 
                                                VkDebugUtilsMessengerEXT debugMessenger, 
                                                const VkAllocationCallbacks* pAllocator);
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void* pUserData);
            /** Vulkan instance handle. */
            VkInstance instance;

            /** Handle to the physical device (GPU) used for rendering. */
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

            /** Reference to the window for Vulkan surface creation. */
            Window& window;

            /** Command pool for allocating command buffers for the graphics queue. */
            VkCommandPool commandPool;

            /** Vulkan debug messenger handle. */
            VkDebugUtilsMessengerEXT debugMessenger;

            /** Logical device handle for interaction with the GPU. */
            VkDevice device_;

            /** Vulkan surface handle for the window. */
            VkSurfaceKHR windowSurface_;

            /** Queue for graphics operations. */
            VkQueue graphicsQueue_;

            /** Queue for presenting images to the window. */
            VkQueue presentQueue_;

            /** List of validation layers to enable for debugging and validation. */
            std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };
    };
} // JCAT

#endif // DEVICE_SETUP_H