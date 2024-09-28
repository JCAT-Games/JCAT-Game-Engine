#include "deviceSetup.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace JCAT {
    DeviceSetup::DeviceSetup(Window& window) : window(window) {
        createVulkanInstance();
        setupDebugMessenger();
        createWindowSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    DeviceSetup::~DeviceSetup() {
        vkDestroyCommandPool(device_, commandPool, nullptr);
        vkDestroyDevice(device_, nullptr);

        if (enableValidationLayers) {
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, windowSurface_, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    VkCommandPool DeviceSetup::getCommandPool() {
        return commandPool;
    }

    VkDevice DeviceSetup::device() {
        return device_;
    }

    VkSurfaceKHR DeviceSetup::windowSurface() {
        return windowSurface_;
    }

    VkQueue DeviceSetup::graphicsQueue() {
        return graphicsQueue_;
    }

    VkQueue DeviceSetup::presentQueue() {
        return presentQueue_;
    }

    void DeviceSetup::createVulkanInstance() {
        // Check if validation layers are requested
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not supported!");
        }

        // Defining the Vulkan application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "JCAT Game Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        // Defining the instance creation information
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;

        // Get the required GLFW extensions
        std::vector<const char*> extensions = getRequiredGLFWExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Enable Validation Layers and Debug Messenger
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        // Create the Vulkan Instance
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the Vulkan Instance!");
        }

        hasGLFWRequiredInstanceExtensions();
    }

    void DeviceSetup::createWindowSurface() {
        
    }

    void DeviceSetup::pickPhysicalDevice() {

    }

    void DeviceSetup::createLogicalDevice() {

    }

    void DeviceSetup::createCommandPool() {

    }

    void DeviceSetup::setupDebugMessenger() {
        if (!enableValidationLayers) {
            return;
        }
        else {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;

            populateDebugMessengerCreateInfo(createInfo);

            if (createDebugUtilsMessenger(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("Failed to set up Debug Messenger!");
            }
        }
    }

    bool DeviceSetup::isDeviceSuitable(VkPhysicalDevice device) {

    }

    std::vector<const char*> DeviceSetup::getRequiredGLFWExtensions() {

    }

    bool DeviceSetup::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availibleLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availibleLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const VkLayerProperties& layerProperties : availibleLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void DeviceSetup::hasGLFWRequiredInstanceExtensions() {

    }

    QueueFamilyIndices DeviceSetup::findQueueFamilies(VkPhysicalDevice device) {

    }

    void DeviceSetup::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }

    bool DeviceSetup::checkDeviceExtensionSupport(VkPhysicalDevice device) {

    }

    VkResult DeviceSetup::createDebugUtilsMessenger(VkInstance instance, 
                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                                const VkAllocationCallbacks* pAllocator, 
                                                VkDebugUtilsMessengerEXT* pDebugMessenger) {
        PFN_vkCreateDebugUtilsMessengerEXT func = 
            (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr) {
            VkResult result = func(instance, pCreateInfo, pAllocator, pDebugMessenger);

            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to set up Debug Messenger!");
            }
        }
        else {
            throw std::runtime_error("Could not load vkCreateDebugUtilsMessengerEXT!");
        }
    }

    void DeviceSetup::destroyDebugUtilsMessengerEXT(VkInstance instance, 
                                                    VkDebugUtilsMessengerEXT debugMessenger, 
                                                    const VkAllocationCallbacks* pAllocator) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = 
            (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
        else {
            throw std::runtime_error("Could not load vkDestroyDebugUtilsMessengerEXT!");
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DeviceSetup::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void* pUserData) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                std::cout << "Validation Layer VERBOSE: " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                std::cout << "Validation Layer INFO: " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                std::cerr << "Validation Layer WARNING: " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cerr << "Validation Layer ERROR: " << pCallbackData->pMessage << std::endl;
                break;
        }

        return VK_FALSE;
    }
}