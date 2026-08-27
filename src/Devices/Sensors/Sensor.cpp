#include <Devices/Sensors/Sensor.hpp>

#include <Vector2.hpp>
#include <Object.hpp>
#include <World.hpp>
#include <Robot.hpp>
#include <Factory.hpp>
#include <Device.hpp>

#include <memory>
#include <utility>

Sensor::Sensor(const std::string& id, const std::string& type) 
    : Device(id, type) {}

Sensor::Sensor(std::unique_ptr<SensorShape> shape, Transform localTransform,
    const std::string& id, const std::string& type) 
    : Device(id, type), shape_(std::move(shape)), localTransform_(localTransform) {}

nlohmann::json Sensor::serialize() const noexcept {
    nlohmann::json out = Device::serialize();

    out["data"]["localTransform"] = localTransform_.serialize();
    out["data"]["shape"] = shape_->serialize();

    return out;
}

void Sensor::deserialize(const nlohmann::json& json) {
    Device::deserialize(json);

    localTransform_.deserialize(json.at("data").at("localTransform"));
    
    // type is found in shape directly, and origin is inside the data
    nlohmann::json shape = json.at("data").at("shape");
    shape_ = std::move(SensorShape::SensorShape_Factory.create(shape.at("type").get<std::string>(), Transform(shape.at("data").at("origin"))));
    shape_->deserialize(shape);
}

SensorShape& Sensor::shape() {
    return *shape_;
}

const SensorShape& Sensor::shape() const noexcept {
    return *shape_;
}

Transform& Sensor::localTransform() {
    return localTransform_;
}

const Transform& Sensor::localTransform() const noexcept {
    return localTransform_;
}

void Sensor::update(double deltaTime) {
    sense();
}

ViewSensor::ViewSensor(const std::string& id, const std::string& type) 
    : Sensor(id, type) {}

ViewSensor::ViewSensor(const std::string& id, double fov, double range,
    Transform localTransform, const std::string& type) 
    : Sensor(std::make_unique<SensorShapeCone>(Transform(Vector2{0, 0}, 0), fov, range), localTransform, id, type) {}

ViewSensor::ViewSensor(const std::string& id, std::unique_ptr<SensorShape> shape,
    Transform localTransform, const std::string& type)
        : Sensor(std::move(shape), localTransform, id, type) {}

void ViewSensor::registerViewSensor() {
    Device::Device_Factory.registerType<ViewSensor>("ViewSensor");
}

Transform ViewSensor::worldTransform() const {
    const Transform& t = robot_->transform();

    return Transform{
        t.position.x + localTransform_.position.x,
        t.position.y + localTransform_.position.y,
        std::fmod(t.rotation + localTransform_.rotation,360.0)
    };
}

double& ViewSensor::fov() {
    return dynamic_cast<SensorShapeCone*>(shape_.get())->fov();
}

const double& ViewSensor::fov() const noexcept {
    return dynamic_cast<SensorShapeCone*>(shape_.get())->fov();
}

double& ViewSensor::range() {
    return dynamic_cast<SensorShapeCone*>(shape_.get())->range();
}

const double& ViewSensor::range() const noexcept {
    return dynamic_cast<SensorShapeCone*>(shape_.get())->range();
}

void ViewSensor::sense() {
    if (!robot_)
        throw std::runtime_error("ViewSensor is not attached to a robot");

    const Transform& robotTransform = robot_->transform();

    const double cosTheta = std::cos(robotTransform.rotation);
    const double sinTheta = std::sin(robotTransform.rotation);

    const double worldX = robotTransform.position.x +
        localTransform_.position.x * cosTheta -
        localTransform_.position.y * sinTheta;

    const double worldY = robotTransform.position.y +
        localTransform_.position.x * sinTheta +
        localTransform_.position.y * cosTheta;

    const double worldRotation = robotTransform.rotation + localTransform_.rotation;

    SensorShapeCone worldShape(
        Transform{{worldX, worldY}, worldRotation},
        fov(),
        range()
    );

    image_ = Image(robot_->world().sense(worldShape));
}

const Image& ViewSensor::image() const noexcept {
    return image_;
}