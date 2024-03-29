// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <open_atmos/mechanism_configuration/parse_status.hpp>
#include <open_atmos/types.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    class JsonParser
    {
     public:
      /// @brief Reads a configuration from a json object
      /// @param object a json object
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const nlohmann::json &object);

      /// @brief Reads a configuration from a file path
      /// @param file_path A path to single json configuration
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const std::filesystem::path &file_path);

      /// @brief Reads a configuration from a file path
      /// @param file_path A path to single json configuration
      /// @return A pair containing the parsing status and mechanism
      std::pair<ConfigParseStatus, types::Mechanism> Parse(const std::string &file_path);
    };
  }  // namespace mechanism_configuration
}  // namespace open_atmos
