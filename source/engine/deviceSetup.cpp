#include "deviceSetup.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <set>
#include <unordered_set>

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

    SwapChainSupportDetails DeviceSetup::getSwapChainSupport() {
        return querySwapChainSupport(physicalDevice);
    }

    uint32_t DeviceSetup::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type!");
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
        std::cout << "Choosing the following GPU: " << deviceProperties.deviceName << std::endl;
        properties = deviceProperties;
    }

    void DeviceSetup::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
    }

    void DeviceSetup::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
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

        // Checks for swap chain support
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        
        bool samplerAnisotropySupported = supportedFeatures.samplerAnisotropy;
        bool sampleRateSahdingSupported = supportedFeatures.sampleRateShading;
        bool geometryShaderSupported = supportedFeatures.geometryShader;

        return indices.isComplete() && extensionsSupported && samplerAnisotropySupported && sampleRateSahdingSupported && geometryShaderSupported;
    }

    std::vector<const char*> DeviceSetup::getRequiredGLFWExtensions() {
        uint32_t glfwRequiredExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

        std::vector<const char*> glfwRequiredExtensions(glfwExtensions, glfwExtensions + glfwRequiredExtensionCount);
        
        if (enableValidationLayers) {
            glfwRequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return glfwRequiredExtensions;
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
        uint32_t glfwExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(glfwExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, extensions.data());

        std::cout << "Vulkan Instance Availible Extensions: " << std::endl;
        std::unordered_set<std::string> availibleExtensions;
        for (const VkExtensionProperties& extension : extensions) {
            std::cout << "\t" << extension.extensionName << std::endl;
            availibleExtensions.insert(extension.extensionName);
        }

        std::vector<const char*> glfwRequiredExtensions = getRequiredGLFWExtensions();

        std::cout << "Required Extensions:" << std::endl;
        for (const char* required : glfwRequiredExtensions) {
            std::cout << "\t" << required << std::endl;
            if (availibleExtensions.find(required) == availibleExtensions.end()) {
                throw std::runtime_error("Missing required GLFW extension");
            }
        }
    }

    QueueFamilyIndices DeviceSetup::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, windowSurface_, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
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

    SwapChainSupportDetails DeviceSetup::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails swapChainDetails;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface_, &swapChainDetails.capabilities);
    
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface_, &formatCount, nullptr);
    
        if (formatCount != 0) {
            swapChainDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface_, &formatCount, swapChainDetails.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface_, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            swapChainDetails.presentModes.resize(presentModeCount);

            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                windowSurface_,
                &presentModeCount,
                swapChainDetails.presentModes.data());
        }

        return swapChainDetails;
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

            return result;
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
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