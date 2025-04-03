#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include "./3d/camera3D.h"

#include <vulkan/vulkan.h>

namespace JCAT {
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera3D &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}

#endif

