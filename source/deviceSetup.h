#ifndef DEVICE_SETUP_H
#define DEVICE_SETUP_H

#include "window.h"

#include <cstdint>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__)
    #include <fstream>
    #include <string>
#endif

namespace JCAT {
    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;

        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;

        bool isComplete() {
            return graphicsFamilyHasValue && presentFamilyHasValue;
        }
    };

    class DeviceSetup {
        public:
            DeviceSetup(Window& window);
            ~DeviceSetup();

            DeviceSetup(const DeviceSetup&) = delete;
            void operator=(const DeviceSetup&) = delete;
            DeviceSetup(DeviceSetup&&) = delete;
            DeviceSetup& operator=(DeviceSetup&&) = delete;

            VkCommandPool getCommandPool();
            VkDevice device();
            VkSurfaceKHR windowSurface();
            VkQueue graphicsQueue();
            VkQueue presentQueue();
        private:
            #ifndef NDEBUG
                const bool enableValidationLayers = false;
            #else
                const bool enableValidationLayers = true;
            #endif

            void createVulkanInstance();
            void createWindowSurface();
            void pickPhysicalDevice();
            void createLogicalDevice();
            void createCommandPool();
            void setupDebugMessenger();

            // Helper functions for main functions
            bool isDeviceSuitable(VkPhysicalDevice device);
            std::vector<const char*> getRequiredGLFWExtensions();
            bool checkValidationLayerSupport();
            void hasGLFWRequiredInstanceExtensions();
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            bool checkDeviceExtensionSupport(VkPhysicalDevice device);
            bool isOnBatteryPower();

            // Debug Messnger Functions
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

            VkInstance instance;
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            Window& window;
            VkCommandPool commandPool;
            VkDebugUtilsMessengerEXT debugMessenger;

            VkDevice device_;
            VkSurfaceKHR windowSurface_;
            VkQueue graphicsQueue_;
            VkQueue presentQueue_;

            std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };
    };
}

#endif