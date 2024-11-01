#include "model2d.h"

namespace JCAT {
    std::vector<VkVertexInputBindingDescription> JCATModel2D::Vertex::getBindingDescriptions() {

    }

    std::vector<VkVertexInputAttributeDescription> JCATModel2D::Vertex::getAttributeDescriptions() {

    }

    JCATModel2D::JCATModel2D(DeviceSetup& d, ResourceManager& r, const std::vector<Vertex>& spriteVertices) : device{d}, resourceManager{r} {

    }

    JCATModel2D::~JCATModel2D() {

    }

    void JCATModel2D::bind(VkCommandBuffer commandBuffer) {

    }

    void JCATModel2D::draw(VkCommandBuffer commandBuffer) {
        
    }
}