#include <Devices/Sensors/SensorShape.hpp>

#include <cmath>
#include <numbers>

SensorShape::SensorShape(Transform origin) : origin_(origin) {}

Transform& SensorShape::origin() {
    return origin_;
}

const Transform& SensorShape::origin() const noexcept {
    return origin_;
}

SensorShapeQuadratic::SensorShapeQuadratic(const std::array<Transform, 4>& vertices) 
    : SensorShape({{0, 0}, 0}), vertices_(order_vertices_(vertices)) {}

const std::array<Transform, 4>& SensorShapeQuadratic::vertices() const noexcept {
    return vertices_;
}

void SensorShapeQuadratic::setVertices(const std::array<Transform, 4>& vertices) {
    vertices_ = order_vertices_(vertices);
}

bool SensorShapeQuadratic::contains(Transform point) const noexcept {
    // Check which side of each edge the point lies on
    bool positive = false;
    bool negative = false;

    for (size_t i = 0; i < 4; ++i) {
        const auto& a = vertices_[i];
        const auto& b = vertices_[(i + 1) % 4];

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

std::array<Transform, 4> SensorShapeQuadratic::order_vertices_(const std::array<Transform, 4>& vertices) {
    std::array<Transform, 4> v = vertices;

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

SensorShapeCone::SensorShapeCone(Transform origin, double fov, double range) 
    : SensorShape(origin), fov_(fov), range_(range) {}

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

SensorShapeBall::SensorShapeBall(Transform origin, double radius) 
    : SensorShape(origin), radius_(radius) {}

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