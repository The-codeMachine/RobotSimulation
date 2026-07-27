#include <World.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {

    try {
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

        world.update(Object({16, 8}, ObjectType::EMPTY));
        world.update(Object({16, 9}, ObjectType::EMPTY));

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

    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }

    return 0;
}