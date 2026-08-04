#pragma once

#include <Device.hpp>
#include <Devices/Motor.hpp>

#include <string>

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
    void update(long long deltaTime) override;

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