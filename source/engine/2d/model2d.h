#ifndef MODEL_TWO_D_H
#define MODEL_TWO_D_H

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"

namespace JCAT {
    class JCATModel2D {
        public:
            struct Vertex2D {
                glm::vec2 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            struct UIVertex {
                glm::vec2 position;
                glm::vec2 uv;
                glm::u8vec4 color;  // Packed normalized RGBA

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            JCATModel2D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex2D>& spriteVertices);
            ~JCATModel2D();

            JCATModel2D(const JCATModel2D&) = delete;
            JCATModel2D& operator=(const JCATModel2D&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex2D>& vertices);

            DeviceSetup& device;
            ResourceManager& resourceManager;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
};

#endif