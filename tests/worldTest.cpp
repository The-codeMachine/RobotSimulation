#include <World.hpp>
#include <Registration.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {
    registerBuiltinObjects();

    try {
        World world(std::filesystem::path("assets/tests/worldInitConstruction.json"));
        
        world.replaceObject(std::make_unique<Empty>(world, Transform({16, 8}, 0), "Empty"));
        world.replaceObject(std::make_unique<Empty>(world, Transform({16, 9}, 0), "Empty"));

        std::filesystem::remove("assets/tests/worldSaveTest.json");
        world.saveToFile("assets/tests/worldSaveTest.json");

        World w(std::filesystem::path("assets/tests/worldSaveTest.json"));
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

return 0;
}