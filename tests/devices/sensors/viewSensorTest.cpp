#include <Registration.hpp>

#include <Devices/Sensors/Sensor.hpp>
#include <Devices/Sensors/SensorShape.hpp>

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {

bool approximatelyEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

void testViewSensorConstruction() {
    ViewSensor sensor("camera", 90.0, 100.0, Transform{});

    assert(sensor.id() == "camera");

    assert(approximatelyEqual(sensor.fov(), 90.0));
    assert(approximatelyEqual(sensor.range(), 100.0));

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);
    assert(approximatelyEqual(cone->fov(), 90.0));
    assert(approximatelyEqual(cone->range(), 100.0));
}

void testViewSensorAccessors() {
    ViewSensor sensor("camera", 90.0, 100.0, Transform{});

    sensor.fov() = 120.0;
    sensor.range() = 250.0;

    assert(approximatelyEqual(sensor.fov(), 120.0));
    assert(approximatelyEqual(sensor.range(), 250.0));

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);
    assert(approximatelyEqual(cone->fov(), 120.0));
    assert(approximatelyEqual(cone->range(), 250.0));
}

void testLocalTransform() {
    Transform transform;

    transform.position = {5.0, 10.0};
    transform.rotation = 2.0;

    ViewSensor sensor("camera", 90.0, 100.0, transform);

    assert(approximatelyEqual(sensor.localTransform().position.x, 5.0));

    assert(approximatelyEqual(sensor.localTransform().position.y, 10.0));

    assert(approximatelyEqual(sensor.localTransform().rotation, 2.0));
}

void testSensorShapeOwnership() {
    Transform origin;
    origin.position = {10.0, 20.0};
    origin.rotation = 1.0;

    auto shape = std::make_unique<SensorShapeCone>(origin, 90.0, 50.0);

    Transform localTransform;
    localTransform.position = {2.0, 3.0};
    localTransform.rotation = 0.5;

    ViewSensor sensor("camera", std::move(shape), localTransform);

    auto* cone = dynamic_cast<SensorShapeCone*>(&sensor.shape());

    assert(cone != nullptr);

    assert(approximatelyEqual(cone->fov(), 90.0));
    assert(approximatelyEqual(cone->range(), 50.0));

    assert(approximatelyEqual(sensor.localTransform().position.x, 2.0));

    assert(approximatelyEqual(sensor.localTransform().position.y, 3.0));

    assert(approximatelyEqual(sensor.localTransform().rotation, 0.5));
}

void testSensorCanOwnQuadraticShape() {
    std::array<Transform, 4> vertices{};

    vertices[0].position = {-1.0, -1.0};
    vertices[1].position = { 1.0, -1.0};
    vertices[2].position = { 1.0,  1.0};
    vertices[3].position = {-1.0,  1.0};

    auto shape = std::make_unique<SensorShapeQuadratic>(vertices);

    ViewSensor sensor("camera", std::move(shape), Transform{});

    auto* quadratic = dynamic_cast<SensorShapeQuadratic*>(&sensor.shape());

    assert(quadratic != nullptr);
}

void testSensorCanOwnBallShape() {
    auto shape = std::make_unique<SensorShapeBall>(Transform{}, 10.0);

    ViewSensor sensor("sensor", std::move(shape), Transform{});

    auto* ball = dynamic_cast<SensorShapeBall*>(&sensor.shape());

    assert(ball != nullptr);
    assert(approximatelyEqual(ball->radius(), 10.0));
}

void testImageInitiallyEmpty() {
    ViewSensor sensor("camera");

    assert(sensor.image().empty());
    assert(sensor.image().size() == 0);
}

void testSerialize() {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;
    ViewSensor sensor("camera", 75.0, 125.0, transform);
    
    nlohmann::json json = sensor.serialize();

    assert(json.contains("data"));

    assert(json["data"]["shape"]["data"]["fov"] == 75.0);
    assert(json["data"]["shape"]["data"]["range"] == 125.0);
}

void testDeserialize() {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;

    ViewSensor original("camera", 75.0, 125.0, transform);

    nlohmann::json json = original.serialize();

    ViewSensor restored("restored");

    restored.deserialize(json);

    assert(approximatelyEqual(restored.fov(), original.fov()));

    assert(approximatelyEqual(restored.range(), original.range()));

    assert(approximatelyEqual(
        restored.localTransform().position.x,
        original.localTransform().position.x
    ));

    assert(approximatelyEqual(
        restored.localTransform().position.y,
        original.localTransform().position.y
    ));

    assert(approximatelyEqual(
        restored.localTransform().rotation,
        original.localTransform().rotation
    ));
}

void testDeserializePreservesShapeType() {
    ViewSensor original("camera", 90.0, 100.0, Transform{});

    nlohmann::json json = original.serialize();

    ViewSensor restored("restored");

    restored.deserialize(json);

    auto* cone = dynamic_cast<SensorShapeCone*>(&restored.shape());

    assert(cone != nullptr);

    assert(approximatelyEqual(cone->fov(), 90.0));
    assert(approximatelyEqual(cone->range(), 100.0));
}

void testSerializeDeserializeRoundTrip() {
    Transform transform;

    transform.position = {4.0, 8.0};
    transform.rotation = 1.25;

    ViewSensor original("camera", 75.0, 125.0, transform);

    nlohmann::json serialized = original.serialize();

    ViewSensor restored("restored");
    restored.deserialize(serialized);

    assert(approximatelyEqual(restored.fov(), 75.0));

    assert(approximatelyEqual(restored.range(), 125.0));

    assert(approximatelyEqual(restored.localTransform().position.x, 4.0));

    assert(approximatelyEqual(restored.localTransform().position.y, 8.0));

    assert(approximatelyEqual(restored.localTransform().rotation, 1.25));

    auto* cone =
        dynamic_cast<SensorShapeCone*>(&restored.shape());

    assert(cone != nullptr);

    assert(approximatelyEqual(cone->fov(), 75.0));

    assert(approximatelyEqual(cone->range(), 125.0));
}

void testFactoryRegistration() {
    auto sensor = Device::Device_Factory.create("ViewSensor", "camera");

    assert(sensor != nullptr);

    auto* viewSensor = dynamic_cast<ViewSensor*>(sensor.get());

    assert(viewSensor != nullptr);
}

} // namespace


int main() {
    registerBuiltinObjects();

    testViewSensorConstruction();
    testViewSensorAccessors();
    testLocalTransform();
    testSensorShapeOwnership();

    testSensorCanOwnQuadraticShape();
    testSensorCanOwnBallShape();
    testImageInitiallyEmpty();

    testSerialize();
    testDeserialize();

    testDeserializePreservesShapeType();
    testSerializeDeserializeRoundTrip();
    testFactoryRegistration();

    return 0;
}