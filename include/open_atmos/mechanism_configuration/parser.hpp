// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <open_atmos/mechanism_configuration/parse_status.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>
#include <open_atmos/types.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    class Parser
    {
     public:
      /// @brief Reads a configuration from a YAML node
      /// @param node a YAML node
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const YAML::Node& node);

      /// @brief Reads a configuration from a file path
      /// @param file_path A path to single YAML configuration
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const std::filesystem::path& file_path);

      /// @brief Reads a configuration from a file path
      /// @param file_path A path to single YAML configuration
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const std::string& file_path);
    };
  }  // namespace mechanism_configuration
}  // namespace open_atmos
