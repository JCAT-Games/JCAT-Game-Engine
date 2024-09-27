#include "deviceSetup.h"

#include <iostream>

namespace JCAT {
    DeviceSetup::DeviceSetup(Window& window) : window(window) {

    }

    DeviceSetup::~DeviceSetup() {

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
}