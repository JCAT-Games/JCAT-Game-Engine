#include "./apps/default/2d/application.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main() {
    JCAT::Application application{};

    try {
        application.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

