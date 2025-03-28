#include "./apps/default/2d/application.h"
#include "./apps/default/3d/application3D.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main() {
    JCAT::Application3D application{};

    try {
        application.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

