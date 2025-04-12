#include "./apps/default/2d/application.h"
#include "./apps/default/3d/application3D.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main() {
    JCAT::Application3D application{};
    //    ^ Set to "Application3D" for 3D Demo Application / "Application" for 2D Demo Application

    try {
        application.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

