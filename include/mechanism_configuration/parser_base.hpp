#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <mechanism_configuration/load_node.hpp>
#include <mechanism_configuration/mechanism.hpp>
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
      if constexpr (IsStringOrPath<T>())
      {
        // version 0 can only take a string or file path
        auto result = TryParse(source);
        if (result)
        {
          auto mechanism = dynamic_cast<MechanismType*>(result->get());
          if (mechanism)
          {
            return *mechanism;
          }
        }
      }

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

    virtual std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const YAML::Node& node) = 0;

    // These are only required for the version 0 parser
    virtual std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const std::filesystem::path& config_path) { return std::nullopt; };
    virtual std::optional<std::unique_ptr<GlobalMechanism>> TryParse(const std::string& config_path) { return std::nullopt; };
  };
}  // namespace mechanism_configuration
