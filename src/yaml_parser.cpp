// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/yaml_parser.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> YamlParser::Parse(const std::string& file_path)
    {
      throw std::runtime_error("Not implemented");
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> YamlParser::Parse(const std::filesystem::path& file_path)
    {
      throw std::runtime_error("Not implemented");
    }

    /// @brief Parse a mechanism
    /// @param node A yaml object representing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> YamlParser::Parse(const YAML::Node& node)
    {
      throw std::runtime_error("Not implemented");
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
