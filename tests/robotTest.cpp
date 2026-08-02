#include <Robot.hpp>
#include <World.hpp>
#include <Registration.hpp>

#include <iostream>
#include <cassert>

class TestDevice : public Device {
public:
    TestDevice(const std::string& id, const std::string& type = "Test_Device") : Device(id, type), counter_(0) {}

    nlohmann::json serialize() const override {
        nlohmann::json json = Device::serialize();

        json["data"]["counter"] = counter_;

        return json;
    }

    void deserialize(const nlohmann::json& json) override {
        Device::deserialize(json);
        counter_ = json.at("data").at("counter");
    }

    void update(long long deltaTime) override {
        counter_ = deltaTime;
    }

    long long counter() const noexcept {
        return counter_;
    }

private:
    long long counter_;

};

int main() {

    registerBuiltinObjects();
    Device::Device_Factory.registerType<TestDevice>("Test_Device");

    World world(std::filesystem::path("assets/tests/robotWorldConstruction.json"));
    Robot& robot = dynamic_cast<Robot&>(world.at({10, 5}));

    assert(world.at({10, 5}).name() == "Robot");

    std::string id = "test_id";

    TestDevice& d = robot.addDevice<TestDevice>(id);
    TestDevice* found = robot.getDevice<TestDevice>(id);

    assert(found != nullptr);
    assert(found == &d);

    for (long long i = 0; i < 5; ++i) {
        robot.update(i);
    }

    assert(d.counter() == 4);

    world.saveToFile("assets/tests/robotSaveTest.json");

    World w(std::filesystem::path("assets/tests/robotSaveTest.json"));
    Robot& r = dynamic_cast<Robot&>(w.at({10, 5}));

    assert(r.getDevice<TestDevice>(id)->counter() == 4);

    return 0;
}