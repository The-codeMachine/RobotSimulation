#include <Robot.hpp>
#include <World.hpp>
#include <Registration.hpp>

#include <iostream>
#include <cassert>

class TestDevice : public Device {
public:
    TestDevice(const std::string& id) : Device(id) {}

    void update(long long deltaTime) override {
        std::cout << "Update: " << deltaTime << "\n";
    }
};

int main() {

    registerBuiltinObjects();
    Device::Device_Factory.registerType<TestDevice>("TestDevice");

    World world(std::filesystem::path("assets/tests/robotWorldConstruction.txt"));
    Robot robot(world, Transform({10, 5}, 0));

    assert(world.at({10, 5}).name() == "Robot");

    std::string id = "test_id";

    TestDevice& d = robot.addDevice<TestDevice>(id);
    TestDevice* found = robot.getDevice<TestDevice>(id);

    assert(found != nullptr);
    assert(found == &d);

    for (long long i = 0; i < 5; ++i) {
        robot.update(i);
    }

    return 0;
}