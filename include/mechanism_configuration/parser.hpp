#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace mechanism_configuration
{
  using GlobalMechanism = ::mechanism_configuration::Mechanism;

  template<typename MechanismType>
  class Parser
  {
   public:
    virtual ~Parser() = default;

    /// @brief Reads a configuration and returns a mechanism
    /// @param source A YAML node, file path, or string representing a file path
    /// @return An optional containing the parsed mechanism
    template<typename T>
    std::optional<MechanismType> Parse(const T& source)
    {
      YAML::Node node = LoadNode(source);
      auto result = TryParse(node);
      if (result)
      {
        auto mechanism = dynamic_cast<MechanismType*>(result->get());
        if (mechanism)
        {
          return *mechanism;  // Dereference the casted pointer
        }
      }
      return std::nullopt;
    }

    /// @brief Tries to read a configuration and returns a mechanism
    /// @param source A YAML node, file path, or string representing a file path
    /// @return An optional containing the parsed mechanism wrapped in a unique pointer
    virtual std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& node) = 0;

   protected:
    /// @brief Helper to load YAML node from a file path, string, or YAML::Node
    /// @param source A file path, string, or YAML::Node
    /// @return A YAML node
    template<typename T>
    YAML::Node LoadNode(const T& source)
    {
      if constexpr (std::is_same_v<std::decay_t<T>, YAML::Node>)
      {
        return source;
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::filesystem::path>)
      {
        // check if the file exists
        if (std::filesystem::exists(source))
        {
          return YAML::LoadFile(source);
        }
        return YAML::Node();
      }
      else
      {
        static_assert(always_false<T>::value, "Unsupported type for LoadNode");
      }
    }

   private:
    // Helper for static_assert to provide meaningful error messages
    template<typename>
    struct always_false : std::false_type
    {
    };
  };
}  // namespace mechanism_configuration