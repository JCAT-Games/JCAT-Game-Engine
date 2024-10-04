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
        window.createWindowSurface(instance, &windowSurface_);
    }

    void DeviceSetup::pickPhysicalDevice() {
        VkPhysicalDevice integratedGPU = VK_NULL_HANDLE;
        VkPhysicalDevice discreteGPU = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::cout << "Device Count: " << deviceCount << std::endl;

        for (const VkPhysicalDevice& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

            if (isDeviceSuitable(device)) {
                if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && discreteGPU == VK_NULL_HANDLE) {
                    discreteGPU = device;
                }
                else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && integratedGPU == VK_NULL_HANDLE) {
                    integratedGPU = device;
                }
            }
        }

        // Should add more sophisticated checking in the future such as shader support
        if (isOnBatteryPower()) {
            if (integratedGPU != VK_NULL_HANDLE) {
                physicalDevice = integratedGPU;
            }
            else {
                physicalDevice = discreteGPU;
            }
        }
        else {
            if (discreteGPU != VK_NULL_HANDLE) {
                physicalDevice = discreteGPU;
            }
            else {
                physicalDevice = integratedGPU;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

        if (integratedGPU != VK_NULL_HANDLE) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(integratedGPU, &deviceProperties);

            std::cout << "Found Integrated GPU: " << deviceProperties.deviceName << std::endl;
        }

        if (discreteGPU != VK_NULL_HANDLE) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(discreteGPU, &deviceProperties);

            std::cout << "Found Discrete GPU: " << deviceProperties.deviceName << std::endl;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        std::cout << "Chose the following GPU: " << deviceProperties.deviceName << std::endl;
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
        QueueFamilyIndices indices = findQueueFamilies(device);
        
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        // Check for swap chain support here

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        
        bool samplerAnisotropySupported = supportedFeatures.samplerAnisotropy;
        bool sampleRateSahdingSupported = supportedFeatures.sampleRateShading;
        bool geometryShaderSupported = supportedFeatures.geometryShader;

        return indices.isComplete() && extensionsSupported && samplerAnisotropySupported && sampleRateSahdingSupported && geometryShaderSupported;
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
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availibleExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availibleExtensions.data());

        std::vector<const char*> requiredExtensions;
        requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        for (const char* required : requiredExtensions) {
            bool extensionFound = false;

            for (const VkExtensionProperties& extension : availibleExtensions) {
                if (strcmp(required, extension.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound) {
                std::cerr << "Required device extension not supported: " << required << std::endl;
                return false;
            }
        }

        return true;
    }

    bool DeviceSetup::isOnBatteryPower() {
        #if defined(_WIN32) || defined(_WIN64)
            SYSTEM_POWER_STATUS powerStatus;

            if (GetSystemPowerStatus(&powerStatus)) {
                return powerStatus.ACLineStatus == 0;
            }

            return false;
        #elif defined(__linux__)
            std::ifstream file("/sys/class/power_supply/AC/online");
            std::string status;

            if (file.is_open()) {
                std::getline(file, status);
                file.close();
                return status == "0"; 
            }
            
            return false;
        #endif
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