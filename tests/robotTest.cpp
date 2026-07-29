#include <Robot.hpp>
#include <World.hpp>

#include <iostream>
#include <cassert>

class TestDevice : public Device {
public:
    TestDevice(const std::vector<unsigned char>& id) : Device(id) {}

    void update(long long deltaTime) override {
        std::cout << "Update: " << deltaTime << "\n";
    }
};

int main() {

    World world(std::filesystem::path("assets/tests/robotWorldConstruction.txt"));
    Robot robot(world, Transform({10, 5}, 0));

    assert(world.at({10, 5}).getType() == ObjectType::ROBOT);

    std::string str = "test_id";
    std::vector<unsigned char> id(str.begin(), str.end());

    TestDevice& d = robot.addDevice<TestDevice>(id);
    TestDevice* found = robot.getDevice<TestDevice>(id);

    assert(found != nullptr);
    assert(found == &d);

    for (long long i = 0; i < 5; ++i) {
        robot.update(i);
    }

    return 0;
}