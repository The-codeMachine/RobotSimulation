#include <Registration.hpp>

#include <World.hpp>

#include <Devices/Sensors/Sensor.hpp>
#include <Devices/Sensors/SensorShape.hpp>

#include <utility/utility.hpp>

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {

void testViewSensorConstruction(Robot& robot) {
    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("construction_sensor", 90.0, 100.0, Transform{})));

    assert(sensor.id() == "construction_sensor");

    assertNear(sensor.fov(), 90.0);
    assertNear(sensor.range(), 100.0);

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);
    assertNear(cone->fov(), 90.0);
    assertNear(cone->range(), 100.0);
}

void testViewSensorAccessors(Robot& robot) {
    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("accessor_sensor", 90.0, 100.0, Transform{})));

    sensor.setFov(120.0);
    sensor.setRange(250.0);

    assertNear(sensor.fov(), 120.0);
    assertNear(sensor.range(), 250.0);

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);
    assertNear(cone->fov(), 120.0);
    assertNear(cone->range(), 250.0);
}

void testLocalTransform(Robot& robot) {
    Transform transform;

    transform.position = {5.0, 10.0};
    transform.rotation = 2.0;

    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("localTransform_sensor", 90.0, 100.0, transform)));

    assertNear(sensor.localTransform().position.x, 5.0);
    assertNear(sensor.localTransform().position.y, 10.0);
    assertNear(sensor.localTransform().rotation, 2.0);
}

void testSensorShapeOwnership(Robot& robot) {
    Transform origin;
    origin.position = {10.0, 20.0};
    origin.rotation = 1.0;

    auto shape = std::make_unique<SensorShapeCone>(origin, 90.0, 50.0);

    Transform localTransform;
    localTransform.position = {2.0, 3.0};
    localTransform.rotation = 0.5;

    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("ownership_sensor", std::move(shape), localTransform)));

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);

    assertNear(cone->fov(), 90.0);
    assertNear(cone->range(), 50.0);
    assertNear(sensor.localTransform().position.x, 2.0);
    assertNear(sensor.localTransform().position.y, 3.0);
    assertNear(sensor.localTransform().rotation, 0.5);
}

void testSensorCanOwnQuadraticShape(Robot& robot) {
    std::array<Transform, 4> vertices{};

    vertices[0].position = {-1.0, -1.0};
    vertices[1].position = { 1.0, -1.0};
    vertices[2].position = { 1.0,  1.0};
    vertices[3].position = {-1.0,  1.0};

    auto shape = std::make_unique<SensorShapeQuadratic>(vertices);

    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("quadratic_ownership_sensor", std::move(shape), Transform{})));

    auto* quadratic = dynamic_cast<SensorShapeQuadratic*>(&sensor.shape());

    assert(quadratic != nullptr);
}

void testSensorCanOwnBallShape(Robot& robot) {
    auto shape = std::make_unique<SensorShapeBall>(Transform{}, 10.0);

    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("ball_ownership_sensor", std::move(shape), Transform{})));

    auto* ball = dynamic_cast<SensorShapeBall*>(&sensor.shape());

    assert(ball != nullptr);
    assertNear(ball->radius(), 10.0);
}

void testImageInitiallyEmpty(Robot& robot) {
    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("image_empty_sensor")));

    assert(sensor.image().empty());
    assert(sensor.image().size() == 0);
}

void testSerialize(Robot& robot) {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;
    ViewSensor& sensor = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("serialization_sensor", 75.0, 125.0, transform)));
    
    nlohmann::json json = sensor.serialize();

    assert(json.contains("data"));

    assert(json["data"]["shape"]["data"]["fov"] == 75.0);
    assert(json["data"]["shape"]["data"]["range"] == 125.0);
}

void testDeserialize(Robot& robot) {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;

    ViewSensor& original = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("original_sensor", 75.0, 125.0, transform)));

    nlohmann::json json = original.serialize();
    robot.removeDevice(original.id());

    ViewSensor& restored = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("original_sensor")));

    restored.deserialize(json);

    assertNear(restored.fov(), original.fov());
    assertNear(restored.range(), original.range());

    assertNear(
        restored.localTransform().position.x,
        original.localTransform().position.x
    );

    assertNear(
        restored.localTransform().position.y,
        original.localTransform().position.y
    );

    assertNear(
        restored.localTransform().rotation,
        original.localTransform().rotation
    );
}

void testDeserializePreservesShapeType(Robot& robot) {
    ViewSensor& original = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("preservation_sensor", 90.0, 100.0, Transform{})));

    nlohmann::json json = original.serialize();

    robot.removeDevice(original.id());

    ViewSensor& restored = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("preservation_sensor", 90.0, 100.0, Transform{})));

    restored.deserialize(json);

    auto* cone = dynamic_cast<SensorShapeCone*>(&restored.shape());

    assert(cone != nullptr);

    assertNear(cone->fov(), 90.0);
    assertNear(cone->range(), 100.0);
}

void testSerializeDeserializeRoundTrip(Robot& robot) {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;

    ViewSensor& original = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("sdroundtrip_sensor", 75.0, 125.0, transform)));

    nlohmann::json serialized = original.serialize();

    robot.removeDevice(original.id());

    ViewSensor& restored = dynamic_cast<ViewSensor&>(robot.addDevice(std::make_unique<ViewSensor>("sdroundtrip_sensor")));
    restored.deserialize(serialized);

    assertNear(restored.fov(), 75.0);
    assertNear(restored.range(), 125.0);
    assertNear(restored.localTransform().position.x, 4.0);
    assertNear(restored.localTransform().position.y, 8.0);
    assertNear(restored.localTransform().rotation, 1.25);

    auto* cone = dynamic_cast<SensorShapeCone*>(&restored.shape());

    assert(cone != nullptr);
    assertNear(cone->fov(), 75.0);
    assertNear(cone->range(), 125.0);
}

void testFactoryRegistration() {
    auto sensor = Device::Device_Factory.create("ViewSensor", "camera");

    assert(sensor != nullptr);

    auto* viewSensor = dynamic_cast<ViewSensor*>(sensor.get());

    assert(viewSensor != nullptr);
}

void testWorldSave(World& world) {
    world.saveToFile("assets/tests/viewSensorsSave.json");
    World w(std::filesystem::path("assets/tests/viewSensorsSave.json"));
    
    Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));

    // Does not fully test that the world was saved, simply guesses
    // that if all the sensors are there, and the same then it should've
    // saved. 
    
    assert(robot.getDevice<ViewSensor>("construction_sensor")->fov() == 90.0);
    assert(robot.getDevice<ViewSensor>("accessor_sensor")->fov() == 120.0);
    assert(robot.getDevice<ViewSensor>("localTransform_sensor")->fov() == 90.0);
    assert(robot.getDevice<ViewSensor>("quadratic_ownership_sensor"));
    assert(robot.getDevice<ViewSensor>("ball_ownership_sensor"));
    assert(robot.getDevice<ViewSensor>("image_empty_sensor"));
    assert(robot.getDevice<ViewSensor>("serialization_sensor")->fov() == 75.0);
    assert(robot.getDevice<ViewSensor>("original_sensor")->fov() == 75.0);
    assert(robot.getDevice<ViewSensor>("preservation_sensor")->fov() == 90.0);
    assert(robot.getDevice<ViewSensor>("sdroundtrip_sensor")->fov() == 75.0);
}

} // namespace

int main() {
    try {
        registerBuiltinObjects();

        World world(std::filesystem::path("assets/tests/viewSensorTest.json"));
        Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));
    
        testViewSensorConstruction(robot);
        testViewSensorAccessors(robot);
        testLocalTransform(robot);
        testSensorShapeOwnership(robot);
    
        testSensorCanOwnQuadraticShape(robot);
        testSensorCanOwnBallShape(robot);
        testImageInitiallyEmpty(robot);
    
        testSerialize(robot);
        testDeserialize(robot);
    
        testDeserializePreservesShapeType(robot);
        testSerializeDeserializeRoundTrip(robot);
        testFactoryRegistration();

        testWorldSave(world);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return 0;
}