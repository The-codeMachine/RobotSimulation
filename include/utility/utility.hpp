#pragma once

#include <Vector2.hpp>

#include <cassert>
#include <cmath>

constexpr double EPSILON = 1e-9;

/// @brief Asserts that two values are near each other. It allows the a small
/// amount of difference (the EPSILON)
/// @param actual 
/// @param expected 
/// @param epsilon 
void assertNear(double actual, double expected, double epsilon = EPSILON);

/// @brief Asserts that two vectors are near each other. It allows the a small
/// amount of difference (the EPSILON)
/// @param actual 
/// @param expected 
/// @param epsilon 
void assertVectorNear(const Vector2& actual, const Vector2& expected, double epsilon = EPSILON);

