#include "./engine/deviceSetup.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <set>
#include <unordered_set>

namespace JCAT {
    /// @brief constructor for the DeviceSetup class.
    /// @param window reference to the window object.
    DeviceSetup::DeviceSetup(Window& window) : window(window) {
        createVulkanInstance();
        setupDebugMessenger();
        createWindowSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    /// @brief Destructor for the DeviceSetup class.
    /// Cleans up Vulkan resources and the window surface.
    DeviceSetup::~DeviceSetup() {
        vkDestroyCommandPool(device_, commandPool, nullptr);
        vkDestroyDevice(device_, nullptr);

        if (enableValidationLayers) {
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, windowSurface_, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    /// @brief Returns the commandPool instance.
    VkCommandPool DeviceSetup::getCommandPool() {
        return commandPool;
    }

    /// @brief Returns the Device instance.
    VkDevice DeviceSetup::device() {
        return device_;
    }

    /// @brief Returns the windowSurface instance.
    VkSurfaceKHR DeviceSetup::windowSurface() {
        return windowSurface_;
    }

    VkQueue DeviceSetup::graphicsQueue() {
        return graphicsQueue_;
    }

    VkQueue DeviceSetup::presentQueue() {
        return presentQueue_;
    }

    /// @brief Returns the details about the swap chain support.
    SwapChainSupportDetails DeviceSetup::getSwapChainSupport() {
        return querySwapChainSupport(physicalDevice);
    }

    /// @brief Finds a supported depth format from the given candidates.
    /// @param candidates The list of candidate formats.
    /// @param tiling The image tiling type (linear or optimal).
    /// @param features The format feature flags to check against.
    /// @return The first supported depth format found.
    /// @throws std::runtime_error if no supported depth format is found.
    VkFormat DeviceSetup::findSupportedDepthFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("Failed to find supported depth format for swap chain!");
    }

    /// @brief Finds a suitable memory type for the given type filter and properties.
    /// @param typeFilter The type filter for the memory type.
    /// @param properties The required memory properties (e.g., host visible, device local).
    /// @return The index of the suitable memory type.
    /// @throws std::runtime_error if no suitable memory type is found.
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

    /// @brief Finds the queue families for the physical device.
    /// @return A QueueFamilyIndices structure containing the indices of the graphics and present families.
    QueueFamilyIndices DeviceSetup::findPhysicalQueueFamilies() {
        return findQueueFamilies(physicalDevice);
    }

    /// @brief Creates a Vulkan instance.
    /// @throws std::runtime_error if the instance creation fails.
    /// @throws std::runtime_error if validation layers are requested but not supported.
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

    /// @brief creates a window surface for the Vulkan instance.
    void DeviceSetup::createWindowSurface() {
        window.createWindowSurface(instance, &windowSurface_);
    }

    /// @brief picks the physical device for the Vulkan instance.
    /// @throws std::runtime_error if no suitable GPU is found.
    /// @throws std::runtime_error if the GPU does not support the required extensions.
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
        int discreteScore = 0;
        int integratedScore = 0;
        for (const VkPhysicalDevice& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            int current_score = 0;

            if (isDeviceSuitable(device, current_score)) {
                if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && discreteGPU == VK_NULL_HANDLE && current_score > discreteScore) {
                    discreteGPU = device;
                    discreteScore = current_score;
                }
                else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && integratedGPU == VK_NULL_HANDLE && current_score > integratedScore) {
                    integratedGPU = device;
                    integratedScore = current_score;
                }
            }
        }

        // Should add more sophisticated checking in the future such as shader support
        if (isOnBatteryPower()) {
            if (integratedGPU != VK_NULL_HANDLE) {
                integratedScore += 400;
            }
            else {
                discreteScore += 100;
            }
        }
        else {
            if (discreteGPU != VK_NULL_HANDLE) {
                discreteScore += 100;
            }
            else {
                integratedScore += 100;
            }
        }

        if(integratedScore > discreteScore) {
            physicalDevice = integratedGPU;
            std::cout << "integrated GPU chosen" << std::endl; 
        }
        else {
            physicalDevice = discreteGPU;
            std::cout << "discrete GPU chosen" << std::endl;
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

    /// @brief Creates a logical device for the physical device.
    /// @throws std::runtime_error if the logical device creation fails.
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

    /// @brief Creates a command pool for the logical device.
    /// @throws std::runtime_error if the command pool creation fails.
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

    /// @brief Populates the debug messenger create info structure.
    /// @throws std::runtime_error if the debug messenger creation fails.
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

    /// @brief determines if the device is suitable for the application.
    /// @param device The physical device to check.
    /// @param score The score of the device.
    /// @return True if the device is suitable, false otherwise.
    bool DeviceSetup::isDeviceSuitable(VkPhysicalDevice device, int& score) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        // Checks for swap chain support
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            if(score >= 0){
                score += 100;
            }
        }
        else {
            std::cerr << "Device does not support the required extensions!" << std::endl; //determine which part if any causes a false return
            score = -1;
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        
        bool samplerAnisotropySupported = supportedFeatures.samplerAnisotropy;
        if (!samplerAnisotropySupported) {
            std::cerr << "Device does not support sampler anisotropy!" << std::endl; //determine which part if any causes a false return
            score -= 50;
        }
        else
        {
            if(score >= 0)
            {
                score += 100;
            }
        }
        bool sampleRateShadingSupported = supportedFeatures.sampleRateShading;
        if(!sampleRateShadingSupported)
        {
            std::cerr << "Device does not support sample rate shading!" << std::endl;//determine which part if any causes a false return
            score = -1;
        }
        else
        {
            if(score >= 0)
            {
                score += 100;
            }
        }
        bool geometryShaderSupported = supportedFeatures.geometryShader;
        if(!geometryShaderSupported)
        {
            std::cerr << "Device does not support geometry shaders!" << std::endl;//determine which part if any causes a false return
            score = -1;
        }
        else
        {
            if(score >= 0)
            {
                score += 100;
            }
        }

        return indices.isComplete() && extensionsSupported && sampleRateShadingSupported && geometryShaderSupported;
    }

    /// @brief finds the required GLFW extensions for the Vulkan instance.
    /// @return returns the required GLFW extensions.
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

    /// @brief checks if the validation layers are supported by the instance.
    /// @return returns true if the validation layers are supported, false otherwise.
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

    /// @brief checks if the required GLFW extensions are available.
    /// @throws std::runtime_error if any required GLFW extension is missing.
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

    /// @brief checks if the device supports the required extensions.
    /// @param device the physical device to check.
    /// @return true if the device supports the required extensions, false otherwise. 
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

    /// @brief queries the swap chain support for the given physical device.
    /// @param device The physical device to query.
    /// @return A SwapChainSupportDetails structure containing the swap chain support details.
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

    /// @brief checks if the system is on battery power.
    /// @return true if the system is on battery power, false otherwise.
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

    /// @brief creates a debug utils messenger for the Vulkan instance.
    /// @param instance The Vulkan instance.
    /// @param pCreateInfo The create info for the debug messenger.
    /// @param pAllocator The allocator for the debug messenger.
    /// @param pDebugMessenger The debug messenger to create.
    /// @return VK_SUCCESS if the debug messenger is created successfully, otherwise an error code.
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

    /// @brief destroys the debug utils messenger for the Vulkan instance.
    /// @param instance The Vulkan instance.
    /// @param debugMessenger The debug messenger to destroy.
    /// @param pAllocator The allocator for the debug messenger.
    /// @throws std::runtime_error if the debug messenger destruction fails.
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

    /// @brief the debug callback function for the debug messenger.
    /// @param messageSeverity The severity of the message.
    /// @param messageType The type of the message.
    /// @param pCallbackData The callback data for the message.
    /// @param pUserData The user data for the callback.
    /// @return VK_FALSE to indicate that the message should not be handled further.
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