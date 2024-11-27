#include "./apps/default/application.h"

#include <stdexcept>

namespace JCAT {
    Application::Application() {}

    Application::~Application() {}

    void Application::run () {
        while (!window.shouldWindowClose()) {
            glfwPollEvents();
        }
    }
}