#include <World.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {

    World world(std::filesystem::path("assets/tests/worldInitConstruction.json"));

    assert(world.toString() == 
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "Wall                    Wall\n"
        "Wall     WallWallWallWallWallWallWallWallWallWall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall       Robot      Wall     Wall\n"
        "Wall              WallWallWall WallWallWall\n"
        "Wall               Wall    Wall\n"
        "Wall               Wall    Wall\n"
        "WallWallWallWallWallWallWallWallWall       WallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall"
    );

    world.update(std::make_unique<Object>(world, Transform({16, 8}, 0), " "));
    world.update(std::make_unique<Object>(world, Transform({16, 9}, 0), " "));

    assert(world.toString() == 
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "Wall                    Wall\n"
        "Wall     WallWallWallWallWallWallWallWallWallWall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall       Robot      Wall     Wall\n"
        "Wall              WallWallWall WallWallWall\n"
        "Wall                    Wall\n"
        "Wall                    Wall\n"
        "WallWallWallWallWallWallWallWallWall       WallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall"
    );

    std::filesystem::remove("assets/tests/worldSaveTest.json");
    world.saveToFile("assets/tests/worldSaveTest.json");

    World w(std::filesystem::path("assets/tests/worldSaveTest.json"));
    assert(w.toString() == 
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "Wall                    Wall\n"
        "Wall     WallWallWallWallWallWallWallWallWallWall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall              Wall     Wall\n"
        "Wall       R      Wall     Wall\n"
        "Wall              WallWallWall WallWallWall\n"
        "Wall                    Wall\n"
        "Wall                    Wall\n"
        "WallWallWallWallWallWallWallWallWall       WallWallWallWallWallWall\n"
        "Wall                    Wall\n"
        "WallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWallWall"
    );

    return 0;
}