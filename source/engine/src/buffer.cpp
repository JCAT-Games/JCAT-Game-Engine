/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 *
 * From Brendan Galea's Vulkan Game Engine Tutorial
 * https://youtu.be/may_GMkfs5k?si=_08QhOPz9HdFrqR9
 */

#include "../buffer.h"

#include <cassert>
#include <cstring>

namespace JCAT {
    VkDeviceSize JCATBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }

        return instanceSize;
    }

    JCATBuffer::JCATBuffer(DeviceSetup &d,
                           ResourceManager &r,
                           VkDeviceSize instanceSize,
                           uint32_t instanceCount,
                           VkBufferUsageFlags usageFlags,
                           VkMemoryPropertyFlags memoryPropertyFlags,
                           VkDeviceSize minOffsetAlignment)
        : device{d},
          resourceManager{r},
          instanceSize{instanceSize},
          instanceCount{instanceCount},
          usageFlags{usageFlags},
          memoryPropertyFlags{memoryPropertyFlags} {

        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        resourceManager.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    JCATBuffer::~JCATBuffer() {
        unmap();
        vkDestroyBuffer(device.device(), buffer, nullptr);
        vkFreeMemory(device.device(), memory, nullptr);
    }

    VkResult JCATBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(buffer && memory && "Called map on buffer before create");
        return vkMapMemory(device.device(), memory, offset, size, 0, &mapped);
    }

    void JCATBuffer::unmap() {
        if (mapped) {
            vkUnmapMemory(device.device(), memory);
            mapped = nullptr;
        }
    }

    void JCATBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        assert(mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, bufferSize);
        }
        else {
            char *memOffset = (char *)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult JCATBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkFlushMappedMemoryRanges(device.device(), 1, &mappedRange);
    }

    VkResult JCATBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(device.device(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo JCATBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo { buffer, offset, size };
    }

    void JCATBuffer::writeToIndex(void *data, int index) {
        writeToBuffer(data, instanceSize, index * alignmentSize);
    }

    VkResult JCATBuffer::flushIndex(int index) { 
        return flush(alignmentSize, index * alignmentSize); 
    }

    VkDescriptorBufferInfo JCATBuffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult JCATBuffer::invalidateIndex(int index) {
        return invalidate(alignmentSize, index * alignmentSize);
    }

}