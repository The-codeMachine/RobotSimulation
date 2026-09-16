#include <utility/utility.hpp>

void assertNear(double actual, double expected, double epsilon) {
    assert(std::abs(actual - expected) < epsilon);
}

void assertVectorNear(const Vector2& actual, const Vector2& expected, double epsilon) {
    assertNear(actual.x, expected.x, epsilon);
    assertNear(actual.y, expected.y, epsilon);
}