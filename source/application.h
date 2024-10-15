#ifndef APPLICATION_H
#define APPLICATION_H

#include "./engine/window.h"
#include "./engine/deviceSetup.h"

// All game engine code should be under the "JCAT" namespace!
namespace JCAT {
    class Application {
        public:
            // This is the default width and height for our graphics window
            static constexpr int WINDOW_WIDTH = 1280;
            static constexpr int WINDOW_HEIGHT = 720;

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
            Window window{ WINDOW_WIDTH, WINDOW_HEIGHT, "JCAT Game Engine" };
            DeviceSetup device{window};
    };
}

#endif