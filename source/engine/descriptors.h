/*
 * From Brendan Galea's Vulkan Game Engine Tutorial
 * https://youtu.be/d5p44idnZLQ?si=jU5LJCSK7UFbBJDT
 */

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "./deviceSetup.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace JCAT {
    class JCATDescriptorSetLayout {
        public:
            class Builder {
                public:
                    Builder(DeviceSetup &device) : device{device} {}

                    Builder &addBinding(
                        uint32_t binding,
                        VkDescriptorType descriptorType,
                        VkShaderStageFlags stageFlags,
                        uint32_t count = 1);

                    std::unique_ptr<JCATDescriptorSetLayout> build() const;

                private:
                    DeviceSetup &device;
                    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
            };

            JCATDescriptorSetLayout(DeviceSetup &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
            ~JCATDescriptorSetLayout();

            JCATDescriptorSetLayout(const JCATDescriptorSetLayout &) = delete;
            JCATDescriptorSetLayout &operator=(const JCATDescriptorSetLayout &) = delete;

            VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

        private:
            DeviceSetup &device;
            VkDescriptorSetLayout descriptorSetLayout;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

            friend class JCATDescriptorWriter;
    };

    class JCATDescriptorPool {
        public:
            class Builder {
                public:
                    Builder(DeviceSetup &device) : device{device} {}

                    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
                    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
                    Builder &setMaxSets(uint32_t count);
                    std::unique_ptr<JCATDescriptorPool> build() const;

                private:
                    DeviceSetup &device;
                    std::vector<VkDescriptorPoolSize> poolSizes{};
                    uint32_t maxSets = 1000;
                    VkDescriptorPoolCreateFlags poolFlags = 0;
            };

            JCATDescriptorPool(
                DeviceSetup &device,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
            ~JCATDescriptorPool();

            JCATDescriptorPool(const JCATDescriptorPool &) = delete;
            JCATDescriptorPool &operator=(const JCATDescriptorPool &) = delete;

            bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

            void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

            void resetPool();

        private:
            DeviceSetup &device;
            VkDescriptorPool descriptorPool;

            friend class JCATDescriptorWriter;
    };

    class JCATDescriptorWriter {
        public:
            JCATDescriptorWriter(JCATDescriptorSetLayout &setLayout, JCATDescriptorPool &pool);

            JCATDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
            JCATDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

            bool build(VkDescriptorSet &set);
            void overwrite(VkDescriptorSet &set);

        private:
            JCATDescriptorSetLayout &setLayout;
            JCATDescriptorPool &pool;
            std::vector<VkWriteDescriptorSet> writes;
    };
}

#endif
