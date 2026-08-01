#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <stdexcept>

class World;

namespace Factory {

    /// @brief The Factory class is responsible for constructing subclasses of a common base class from a common string identifier. 
    /// @tparam Base 
    /// @tparam ...Args 
    template<typename Base, typename... Args>
    class Factory {
    public:
        Factory() = default;

        /// @brief Registers type Derived where Derived must be a subclass of Base. 
        /// @tparam Derived 
        /// @param name 
        template <typename Derived> 
            requires std::derived_from<Derived, Base>
        void registerType(std::string_view name) {
            auto [it, inserted] = creators_.try_emplace(std::string(name), &createImpl<Derived>);

            if (!inserted)
                throw std::runtime_error("Type already registered: " + std::string(name));
        }

        /// @brief Creates a new unique_ptr of the Base class based off the type's name
        /// @param name 
        /// @param ...args 
        /// @return nullptr if that type is not registered, else a unique_ptr to the Base class
        std::unique_ptr<Base> create(std::string_view name, Args... args) const {
            auto it = creators_.find(std::string(name));

            if (it == creators_.end())
                return nullptr;

            return it->second(std::forward<Args>(args)...);
        }

        bool contains(std::string_view name) const {
            return creators_.find(std::string(name)) != creators_.end();
        }

    private:
        /// @brief Creates an implication (makes a unique_ptr of the Derived class and forwards the arguments)
        /// @tparam Derived 
        /// @param ...args 
        /// @return a unique pointer of the base class
        template<typename Derived>
        static std::unique_ptr<Base> createImpl(Args... args) {
            return std::make_unique<Derived>(std::forward<Args>(args)...);
        }

    private:
        using Creator = std::unique_ptr<Base> (*)(Args...);

    private:
        std::unordered_map<std::string, Creator> creators_;

    };

} // namespace Factory
