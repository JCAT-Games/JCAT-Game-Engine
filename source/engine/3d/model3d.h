#ifndef MODEL_THREE_D_H
#define MODEL_THREE_D_H

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"

namespace JCAT {
    class JCATModel3D {
        public:
            struct Vertex3D {
                glm::vec3 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            JCATModel3D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex3D> &objectVertices);
            ~JCATModel3D();

            JCATModel3D(const JCATModel3D&) = delete;
            JCATModel3D& operator=(const JCATModel3D&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex3D>& vertices);

            DeviceSetup& device;
            ResourceManager& resourceManager;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
};

#endif