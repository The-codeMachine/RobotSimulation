#include <Devices/Sensors/Image.hpp>

#include <algorithm>
#include <utility>

Image::Image() {}
Image::Image(std::vector<Detection> detections) : detections_(std::move(detections)) {}

std::vector<Detection>& Image::detections() {
    return detections_;
}

const std::vector<Detection>& Image::detections() const noexcept {
    return detections_;
}

Detection& Image::detection(size_t index) {
    if (index >= detections_.size())
        throw std::invalid_argument("Index is too large for this image");

    return detections_[index];
}

const Detection& Image::detection(size_t index) const {
    if (index >= detections_.size()) 
        throw std::invalid_argument("Index is too large for this image");

    return detections_[index];
}

size_t Image::size() const noexcept {
    return detections_.size();
}

bool Image::empty() const noexcept {
    return detections_.empty();
}

void Image::insertDetection(Detection detection) {
    detections_.push_back(std::move(detection));
}

void Image::deleteDetection(size_t index) {
    if (index >= detections_.size())
        throw std::invalid_argument("Index is too large for this image");
    
    detections_.erase(detections_.begin() + index);
}