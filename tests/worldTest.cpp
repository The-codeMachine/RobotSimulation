#include <World.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {

    World world(std::filesystem::path("assets/tests/worldInitConstruction.txt"));

    assert(world.toString() == 
        "######################\n"
        "#                    #\n"
        "#                    #\n"
        "#     ##########     #\n"
        "#              #     #\n"
        "#              #     #\n"
        "#       R      #     #\n"
        "#              ### ###\n"
        "#               #    #\n"
        "#               #    #\n"
        "#########       ######\n"
        "#                    #\n"
        "######################"
    );

    world.update(std::make_unique<Object>(world, Transform({16, 8}, 0), " "));
    world.update(std::make_unique<Object>(world, Transform({16, 9}, 0), " "));

    assert(world.toString() == 
        "######################\n"
        "#                    #\n"
        "#                    #\n"
        "#     ##########     #\n"
        "#              #     #\n"
        "#              #     #\n"
        "#       R      #     #\n"
        "#              ### ###\n"
        "#                    #\n"
        "#                    #\n"
        "#########       ######\n"
        "#                    #\n"
        "######################"
    );

    std::filesystem::remove("assets/tests/worldSaveTest.txt");
    world.saveToFile("assets/tests/worldSaveTest.txt");

    World w(std::filesystem::path("assets/tests/worldSaveTest.txt"));
    assert(w.toString() == 
        "######################\n"
        "#                    #\n"
        "#                    #\n"
        "#     ##########     #\n"
        "#              #     #\n"
        "#              #     #\n"
        "#       R      #     #\n"
        "#              ### ###\n"
        "#                    #\n"
        "#                    #\n"
        "#########       ######\n"
        "#                    #\n"
        "######################"
    );

    return 0;
}