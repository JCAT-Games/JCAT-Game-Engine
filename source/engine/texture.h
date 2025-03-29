#pragma once

#include "./deviceSetup.h"
#include "./resourceManager.h"
#include <string>

namespace JCAT {

    class Texture {
        public:
            Texture(DeviceSetup &device, ResourceManager &resourceManager, const std::string &filepath);
            ~Texture();

            Texture(const Texture &) = delete;
            Texture &operator=(const Texture &) = delete;
            Texture(Texture &&) = delete;
            Texture &operator=(Texture &&) = delete;

            VkSampler getSampler() { return sampler; }
            VkImageView getImageView() { return imageView; }
            VkImageLayout getImageLayout() { return imageLayout; }

        private:
            void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

            DeviceSetup& device;
            ResourceManager& resourceManager;
            VkImage image;
            VkDeviceMemory imageMemory;
            VkImageView imageView;
            VkSampler sampler;
            VkFormat imageFormat;
            VkImageLayout imageLayout;

    };

}