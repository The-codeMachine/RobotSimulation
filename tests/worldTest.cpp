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

    world.update(Object(world, Transform({16, 8}, 0), ObjectType::EMPTY));
    world.update(Object(world, Transform({16, 9}, 0), ObjectType::EMPTY));

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

    return 0;
}