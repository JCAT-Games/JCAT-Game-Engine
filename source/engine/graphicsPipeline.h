#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include "./engine/deviceSetup.h"

#include <string>
#include <vector>

namespace JCAT {
    class GraphicsPipeline {
        public:
            GraphicsPipeline(DeviceSetup &device, const std::string& vertFilepath, const std::string& fragfilepath);
            ~GraphicsPipeline() {}

            GraphicsPipeline(const GraphicsPipeline&) = delete;
            void operator=(const GraphicsPipeline&) = delete;
        private:
            static std::vector<char> readFile(const std::string& filepath);

            DeviceSetup &device;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}

#endif