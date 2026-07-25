#include <World.hpp>

#include <iostream>
#include <stdexcept>

int main() {

    try {
        World world(std::filesystem::path("assets/tests/worldInitConstruction.txt"));

        std::cout << world.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }

    return 0;
}