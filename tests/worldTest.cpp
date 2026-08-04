#include <World.hpp>
#include <Registration.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {

    registerBuiltinObjects();

    World world(std::filesystem::path("assets/tests/worldInitConstruction.json"));

    assert(world.toString() == 
        "#######################\n"
        "#                     #\n"
        "#                     #\n"
        "#     ##########      #\n"
        "#              #      #\n"
        "#              #      #\n"
        "#       R      #      #\n"
        "#              ### ####\n"
        "#               #     #\n"
        "#               #     #\n"
        "#########       #######\n"
        "#                     #\n"
        "#######################"
    );

    world.replaceObject(std::make_unique<Empty>(world, Transform({16, 8}, 0), "Empty"));
    world.replaceObject(std::make_unique<Empty>(world, Transform({16, 9}, 0), "Empty"));

    assert(world.toString() == 
        "#######################\n"
        "#                     #\n"
        "#                     #\n"
        "#     ##########      #\n"
        "#              #      #\n"
        "#              #      #\n"
        "#       R      #      #\n"
        "#              ### ####\n"
        "#                     #\n"
        "#                     #\n"
        "#########       #######\n"
        "#                     #\n"
        "#######################"
    );

    std::filesystem::remove("assets/tests/worldSaveTest.json");
    world.saveToFile("assets/tests/worldSaveTest.json");

    World w(std::filesystem::path("assets/tests/worldSaveTest.json"));
    assert(w.toString() == 
        "#######################\n"
        "#                     #\n"
        "#                     #\n"
        "#     ##########      #\n"
        "#              #      #\n"
        "#              #      #\n"
        "#       R      #      #\n"
        "#              ### ####\n"
        "#                     #\n"
        "#                     #\n"
        "#########       #######\n"
        "#                     #\n"
        "#######################"
    );

    return 0;
}