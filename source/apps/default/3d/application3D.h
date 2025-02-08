#ifndef APPLICATION_3D
#define APPLICATION_3D

#include <memory>
#include <vector>

#include "./engine/window.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/3d/gameObject.h"
#include "./engine/renderer.h"

namespace JCAT {
    class Application3D {
        public:
            static constexpr int DEFAULT_WIDTH = 1280;
            static constexpr int DEFAULT_HEIGHT = 720;

            Application3D();
            ~Application3D();

            Application3D(const Application3D&) = delete;
            Application3D& operator=(const Application3D&) = delete;

            void run();
        private:
            void loadGameObjects();

            Window window{ DEFAULT_WIDTH, DEFAULT_HEIGHT, "JCAT Game Engine", true };
            DeviceSetup device{window};
            ResourceManager resourceManager{device};
            Renderer renderer{ window, device, resourceManager, "3D", false };

            std::vector<GameObject> gameObjects;

            bool fKeyPressedLastFrame = false;
    };
};

#endif