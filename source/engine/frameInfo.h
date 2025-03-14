#pragma once

#include "./3d/camera3D.h"

#include <vulkan/vulkan.h>

namespace JCAT {
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera3D &camera;
    };
}

