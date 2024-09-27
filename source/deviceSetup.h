#ifndef DEVICE_SETUP_H
#define DEVICE_SETUP_H

#include "window.h"

#include <cstdint>
#include <vector>

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
            void createVulkanInstance();
            void createWindowSurface();
            void pickPhysicalDevice();
            void createLogicalDevice();
            void createCommandPool();
            void setupDebugMessenger();

            // Helper functions for main functions
            bool isDeviceSuitable(VkPhysicalDevice device);
            std::vector<const char*> getRequiredGLFWExtensions();
            void hasGLFWRequiredInstanceExtensions();
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
            bool checkDeviceExtensionSupport(VkPhysicalDevice device);

            VkInstance instance;
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            Window& window;
            VkCommandPool commandPool;
            VkDebugUtilsMessengerEXT debugMessenger;

            VkDevice device_;
            VkSurfaceKHR windowSurface_;
            VkQueue graphicsQueue_;
            VkQueue presentQueue_;
    };
}

#endif