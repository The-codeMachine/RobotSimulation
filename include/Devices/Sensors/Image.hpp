#pragma once

#include <Object.hpp>

#include <vector>

/// @brief This encapsulates what object was detected
/// and where it was detected. 
struct Detection {
    std::unique_ptr<Object> object;
    Vector2 position;
};

/// @brief Image represents a list of Detections. Image allows 
/// access to its vector of Detections. Image simply 
/// encapsulates these Detections.
class Image {
public:
    Image();
    Image(std::vector<Detection> detections);

    /// @brief Gets all the detections
    /// @return the detections as a reference
    std::vector<Detection>& detections();
    
    /// @brief Gets all the detections
    /// @return the detections as a const reference
    const std::vector<Detection>& detections() const noexcept;
    
    /// @brief Gets a detection at index if possible, throws if it does not exist
    /// @param index 
    /// @return the detection as a reference
    Detection& detection(size_t index);
    
    /// @brief Gets a detection at index if possible, throws if it does not exist
    /// @param index 
    /// @return the detection as a const reference
    const Detection& detection(size_t index) const;

    /// @brief Gets the size of the Image
    /// @return the size of the image
    size_t size() const noexcept;
    
    /// @brief Checks whether this Image is empty
    /// @return true if this Image is empty
    bool empty() const noexcept;

    /// @brief Inserts a detection to the end of detections
    /// @param detection 
    void insertDetection(Detection detection);

    /// @brief Deletes a detection at a certain index
    /// @param index 
    void deleteDetection(size_t index);

private:
    std::vector<Detection> detections_;

};