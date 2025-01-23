// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/parser.hpp>

namespace mechanism_configuration
{

  /// @brief Parse a mechanism
  /// @param file_path a location on the hard drive containing a mechanism
  /// @return A pair containing the parsing status and a mechanism
  std::pair<ConfigParseStatus, MechanismVersions> Parser::Parse(const std::string& file_path)
  {
    return Parser::Parse(std::filesystem::path(file_path));
  }

  /// @brief Parse a mechanism
  /// @param file_path a location on the hard drive containing a mechanism
  /// @return A pair containing the parsing status and a mechanism
  std::pair<ConfigParseStatus, MechanismVersions> Parser::Parse(const std::filesystem::path& file_path)
  {
    ConfigParseStatus status;

    if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path))
    {
      status = ConfigParseStatus::InvalidFilePath;
      std::string msg = configParseStatusToString(status);
      std::cerr << msg << std::endl;
      return { status, v1::Mechanism() };
    }

    YAML::Node config = YAML::LoadFile(file_path.string());

    return Parser::Parse(config);
  }

  /// @brief Parse a mechanism of any supported version. Which version return
  /// @param node A yaml object representing a mechanism
  /// @return A pair containing the parsing status and a mechanism
  std::pair<ConfigParseStatus, MechanismVersions> Parser::Parse(const YAML::Node& object)
  {
    // Attempt to parse the node from the most recent version, to the oldest
    // The return type will be whichever version first successfully parses

    v0::Parser v0_parser;
    v1::Parser v1_parser;

    ConfigParseStatus status;
    v1::Mechanism v1_mechanism;
    v0::Mechanism v0_mechanism;

    MechanismVersions mechanism;


    [status, v1_mechanism] = v1_parser.Parse(object);

    if (status != ConfigParseStatus::InvalidVersion) {
      mechanism.emplace(v1_mechanism);
      return {status, mechanism};
    }

    [status, v0_mechanism] = v0_parser.Parse(object);
    mechanism.emplace(v0_mechanism);


    return {status, mechanism};
  }
}  // namespace mechanism_configuration