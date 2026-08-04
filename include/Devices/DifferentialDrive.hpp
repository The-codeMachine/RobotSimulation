#pragma once

#include <Collision.hpp>

#include <Device.hpp>
#include <Devices/Motor.hpp>

#include <string>

/// @brief Defines the DifferentialDrive's Trajectory. This
/// extends from the Trajectory class. Calculates the
/// location, velocity, and rotation of the DifferentialDrive. 
class DifferentialDriveTrajectory final : public Trajectory {
public:
    DifferentialDriveTrajectory(Vector2 position, double rotation, double linearVelocity,
                                double angularVelocity, double deltaTime);

    /// @brief Gets the position of the trajectory based off the time
    /// @param t 
    /// @return the position of the trajectory based off the time
    Vector2 position(double t) const override;

    /// @brief Gets the velocity of the trajectory based off the time
    /// @param t 
    /// @return the velocity of the trajectory based off the time
    Vector2 velocity(double t) const override;

    /// @brief Gets the rotation of the trajectory based off the time
    /// @param t 
    /// @return the rotation of the trajectory based off the time
    double rotation(double t) const;

private:
    Vector2 position_;
    double rotation_;
    double linearVelocity_;
    double angularVelocity_;
    double deltaTime_;

};

/// @brief This is a differential drive device. It references two motors which it converts
/// their wheel velocities to linear/angular velocities which affect how the Robot moves.
/// The Motors and this drive work separately. 
class DifferentialDrive : public Device {
public:
    DifferentialDrive(const std::string& id, const std::string& type = "DifferentialDrive");
    DifferentialDrive(const std::string& id, const std::string& leftMotorId, const std::string& rightMotorId,
                      double wheelRadius, double wheelBase, const std::string& type = "DifferentialDrive");

    /// @brief Registers the differential drive into the devices factory
    static void registerDifferentialDrive();

    /// @brief Gets the left and right motors from the robot based off the inputted ids
    /// @param robot 
    void onAttach(Robot& robot) override;
    
    /// @brief updates the Robot's transform, and the drive's velocities
    /// @param deltaTime 
    void update(double deltaTime) override;

    nlohmann::json serialize() const override;
    void deserialize(const nlohmann::json& json) override;

    /// @brief gets and returns the linear velocity of the drive
    /// @return the linear velocity of the drive
    double getLinearVelicity() const noexcept;

    /// @brief gets and returns the angular velocity of the drive
    /// @return the angular velocity of the drive
    double getAngularVelocity() const noexcept;

    /// @brief the update priority of the DifferentialDevice, 50 so that it is ahead of motors
    /// @return 50 
    int updatePriority() const noexcept override;

private:
    std::string leftMotorId_;
    std::string rightMotorId_;

    Motor* leftMotor_;
    Motor* rightMotor_;

    double wheelRadius_;
    double wheelBase_;

    double linearVelocity_;
    double angularVelocity_;

};