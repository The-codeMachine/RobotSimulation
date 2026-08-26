#include <Devices/Sensors/SensorShape.hpp>

#include <cmath>
#include <numbers>

SensorShape::SensorShape(const std::string& type) 
    : type_(type) {}

SensorShape::SensorShape(Transform origin, const std::string& type) 
    : origin_(origin), type_(type) {}

nlohmann::json SensorShape::serialize() const noexcept {
    nlohmann::json out;

    out["type"] = type_;
    out["data"]["origin"] = origin_.serialize();

    return out;
}

void SensorShape::deserialize(const nlohmann::json& json) {
    type_ = json.at("type");
    origin_.deserialize(json.at("data").at("origin"));
}

Transform& SensorShape::origin() {
    return origin_;
}

const Transform& SensorShape::origin() const noexcept {
    return origin_;
}

SensorShapeQuadratic::SensorShapeQuadratic(Transform origin, const std::string& type) 
    : SensorShape(origin, type), vertices_({Transform(), Transform(), Transform(), Transform()}) {}

SensorShapeQuadratic::SensorShapeQuadratic(const std::array<Transform, NUMBER_OF_VERTICES>& vertices, const std::string& type) 
    : SensorShape({{0, 0}, 0}, type), vertices_(order_vertices_(vertices)) {}

void SensorShapeQuadratic::registerSensorShapeQuadratic() {
    SensorShape_Factory.registerType<SensorShapeQuadratic>("SensorShapeQuadratic");
}

nlohmann::json SensorShapeQuadratic::serialize() const noexcept {
    nlohmann::json out = SensorShape::serialize();

    for (size_t i = 0; i < 4; ++i) {
        out["data"]["coordinate" + std::to_string(i)] = vertices_[i].serialize();
    }

    return out;
}

void SensorShapeQuadratic::deserialize(const nlohmann::json& json) {
    SensorShape::deserialize(json);

    for (size_t i = 0; i < 4; ++i) {
        vertices_[i].deserialize(json.at("data").at("coordinate" + std::to_string(i)));
    }
}

const std::array<Transform, 4>& SensorShapeQuadratic::vertices() const noexcept {
    return vertices_;
}

void SensorShapeQuadratic::setVertices(const std::array<Transform, NUMBER_OF_VERTICES>& vertices) {
    vertices_ = order_vertices_(vertices);
}

bool SensorShapeQuadratic::contains(Transform point) const noexcept {
    // Check which side of each edge the point lies on
    bool positive = false;
    bool negative = false;

    for (size_t i = 0; i < NUMBER_OF_VERTICES; ++i) {
        const auto& a = vertices_[i];
        const auto& b = vertices_[(i + 1) % NUMBER_OF_VERTICES];

        const double cross = 
            (b.position.x - a.position.x) * (point.position.y - a.position.y) -
            (b.position.y - a.position.y) * (point.position.x - a.position.x);

        if (cross > 0.0)
            positive = true;
        else if (cross < 0.0)
            negative = true;

        // Point is outside if it on both sides of the polygon
        if (positive && negative)
            return false;
    }

    return true;
}

std::array<Transform, SensorShapeQuadratic::NUMBER_OF_VERTICES> SensorShapeQuadratic::order_vertices_(const std::array<Transform, NUMBER_OF_VERTICES>& vertices) {
    std::array<Transform, NUMBER_OF_VERTICES> v = vertices;

    // Find the centroid of the vertices.
    double centerX = 0.0;
    double centerY = 0.0;

    for (const auto& vertex : v) {
        centerX += vertex.position.x;
        centerY += vertex.position.y;
    }

    centerX /= 4.0;
    centerY /= 4.0;

    // Sort vertices counter-clockwise around the center.
    std::sort(v.begin(), v.end(),
        [centerX, centerY](const Transform& a, const Transform& b) {
            const double angleA = std::atan2(a.position.y - centerY, a.position.x - centerX);

            const double angleB = std::atan2(b.position.y - centerY, b.position.x - centerX);

            return angleA < angleB;
        });

    return v;
}

SensorShapeCone::SensorShapeCone(Transform origin, const std::string& type)
    : SensorShape(origin, type), fov_(0), range_(0) {}

SensorShapeCone::SensorShapeCone(Transform origin, double fov, double range, const std::string& type) 
    : SensorShape(origin, type), fov_(fov), range_(range) {}

void SensorShapeCone::registerSensorShapeCone() {
    SensorShape_Factory.registerType<SensorShapeCone>("SensorShapeCone");
}

nlohmann::json SensorShapeCone::serialize() const noexcept {
    nlohmann::json out = SensorShape::serialize();

    out["data"]["fov"] = fov_;
    out["data"]["range"] = range_;

    return out;
}

void SensorShapeCone::deserialize(const nlohmann::json& json) {
    SensorShape::deserialize(json);

    fov_ = json.at("data").at("fov");
    range_ = json.at("data").at("range");
}

double& SensorShapeCone::fov() {
    return fov_;
}

const double& SensorShapeCone::fov() const noexcept {
    return fov_;
}

double& SensorShapeCone::range() {
    return range_;
}

const double& SensorShapeCone::range() const noexcept {
    return range_;
}

bool SensorShapeCone::contains(Transform point) const noexcept {
    const double dx = point.position.x - origin_.position.x;
    const double dy = point.position.y - origin_.position.y;

    const double distanceSquared = dx * dx + dy * dy;

    // Outside range
    if (distanceSquared > range_ * range_)
        return false;

    // Point is exactly at the origin
    if (distanceSquared == 0.0)
        return true;

    // Sensor's facing direction
    const double rotation = std::numbers::pi / 180.0 * origin_.rotation;

    const double facingX = std::cos(rotation);
    const double facingY = std::sin(rotation);

    // Normalize point direction
    const double distance = std::sqrt(distanceSquared);

    const double directionX = dx / distance;
    const double directionY = dy / distance;

    // Dot product gives cos(angle) between the two directions
    const double dot = facingX * directionX + facingY * directionY;

    const double halfFOV = std::numbers::pi / 180.0 * (fov_ / 2.0);

    return dot >= std::cos(halfFOV);
}

SensorShapeBall::SensorShapeBall(Transform origin, const std::string& type) 
    : SensorShape(origin, type), radius_(0) {}

SensorShapeBall::SensorShapeBall(Transform origin, double radius, const std::string& type) 
    : SensorShape(origin, type), radius_(radius) {}

void SensorShapeBall::registerSensorShapeBall() {
    SensorShape_Factory.registerType<SensorShapeBall>("SensorShapeBall");
}

nlohmann::json SensorShapeBall::serialize() const noexcept {
    nlohmann::json out = SensorShape::serialize();

    out["data"]["radius"] = radius_;

    return out;
}

void SensorShapeBall::deserialize(const nlohmann::json& json) {
    SensorShape::deserialize(json);

    radius_ = json.at("data").at("radius");
}

double& SensorShapeBall::radius() {
    return radius_;
}

const double& SensorShapeBall::radius() const noexcept {
    return radius_;
}

bool SensorShapeBall::contains(Transform point) const noexcept {
    const double dx = point.position.x - origin_.position.x;
    const double dy = point.position.y - origin_.position.y;

    const double distanceSquared = dx * dx + dy * dy;

    return distanceSquared <= radius_ * radius_;
}