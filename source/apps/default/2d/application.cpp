#include "./apps/default/2d/application.h"

#include "./appCore/keyboardController.h"
#include "./engine/2d/camera2D.h"
#include "./apps/default/2d/applicationRenderer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>
#include <memory>

namespace JCAT {
    Application::Application() {
        loadGameSprites();
    }

    Application::~Application() {}

    void Application::run () {
        ApplicationRenderer applicationRenderer{ device, resourceManager, renderer.getSwapChainrenderPass() };
    
        Camera2D camera{};
        camera.setViewTarget(glm::vec2(0.f, 0.f));
        GameSprite viewerObject = GameSprite::createGameSprite();
        KeyboardController cameraController{};

        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();

        while (!window.shouldWindowClose()) {
            glfwPollEvents();

            std::chrono::time_point<std::chrono::high_resolution_clock> newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // Might need to fix camera movement later on

            float aspect = renderer.getAspectRatio();
            camera.setOrthographicProjection(-aspect, aspect, -1, 1);

            if (VkCommandBuffer commandBuffer = renderer.beginRecordingFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                applicationRenderer.renderGameObjects(commandBuffer, gameSprites, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endRecordingFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    void Application::loadGameSprites() {
        std::vector<JCATModel2D::Vertex2D> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

		std::shared_ptr<JCATModel2D> triangleModel = std::make_shared<JCATModel2D>(device, resourceManager, vertices);

        std::vector<glm::vec3> colors{
            {1.f, .7f, .73f},
            {1.f, .87f, .73f},
            {1.f, 1.f, .73f},
            {.73f, 1.f, .8f},
            {.73, .88f, 1.f}
        };

        for (glm::vec3& color : colors) {
            color = glm::pow(color, glm::vec3{ 2.2f });
        }

        for (int i = 0; i < 40; i++) {
            GameSprite triangle = GameSprite::createGameSprite();
            triangle.model2D = triangleModel;
            triangle.transform.scale = glm::vec2(0.5f) + i * 0.025f;
            triangle.transform.rotation = i * glm::pi<float>() * .025f;
            triangle.color = colors[i % colors.size()];
            gameSprites.push_back(std::move(triangle));
        }
    }
}