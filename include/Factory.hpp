#pragma once

#include "Object.hpp"

#include <unordered_map>
#include <memory>
#include <stdexcept>

class World;

namespace Factory {

    template<typename Base>
    class Factory {
    public:
        template<typename Derived>
        void registerType(std::string_view name) {
            creators_.emplace(name, [](World& w) {
                return std::make_unique<Derived>(w);
            });
        }

    private:
        std::unordered_map<std::string_view, std::unique_ptr<Base>(*)(World&)> creators_;
    };

} // namespace Factory
