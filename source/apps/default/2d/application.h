#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <vector>

#include "./engine/window.h"
#include "./engine/deviceSetup.h"
#include "./engine/resourceManager.h"
#include "./engine/2d/gameSprite.h"
#include "./engine/renderer.h"

// All game engine code should be under the "JCAT" namespace!
namespace JCAT {
    class Application {
        public:
            // This is the default width and height for our graphics window
            static constexpr int DEFAULT_WIDTH = 1280;
            static constexpr int DEFAULT_HEIGHT = 720;

            // Constructor
            Application();

            // Destructor
            ~Application();

            // Make sure an instance of this class CANNOT be copied
            Application(const Application &) = delete;
            Application& operator=(const Application&) = delete;

            // Running the application
            void run();
        private:
            void loadGameSprites();

            Window window{ DEFAULT_WIDTH, DEFAULT_HEIGHT, "JCAT Game Engine" };
            DeviceSetup device{window};
            ResourceManager resourceManager{device};

            std::vector<GameSprite> gameSprites;
    };
}

#endif