#pragma once

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/mechanism.hpp>
#include <mechanism_configuration/load_node.hpp>

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace mechanism_configuration
{
  using GlobalMechanism = ::mechanism_configuration::Mechanism;

  template<typename MechanismType = GlobalMechanism>
  class ParserBase
  {
   public:
    virtual ~ParserBase() = default;

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
  };
}  // namespace mechanism_configuration