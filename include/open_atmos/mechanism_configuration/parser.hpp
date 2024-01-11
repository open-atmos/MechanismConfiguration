// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

#include <open_atmos/types.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    enum class ConfigParseStatus
    {
      Success,
      None,
      InvalidKey,
      UnknownKey,
      InvalidFilePath,
      NoConfigFilesFound,
      FilesSectionNotFound,
      DataSectionNotFound,
      InvalidSpecies,
      InvalidMechanism,
      ObjectTypeNotFound,
      RequiredKeyNotFound,
      ContainsNonStandardKey,
      MutuallyExclusiveOption
    };
    std::string configParseStatusToString(const ConfigParseStatus &status);

    struct Mechanism
    {    
      std::vector<types::Species> species_arr_;
      std::unordered_map<std::string, types::Phase> phases_;
    };

    class JsonReaderPolicy
    {
    public:
      /// @brief Parse configures
      /// @param config_path Path to a the CAMP configuration directory or file
      /// @return True for successful parsing
      std::pair<ConfigParseStatus, Mechanism> Parse(const std::filesystem::path &config_path);
    };

    /// @brief Public interface to read and parse config
    template <class ConfigTypePolicy = JsonReaderPolicy>
    class ConfigurationReader : public ConfigTypePolicy
    {
    public:
      /// @brief Reads and parses configures
      /// @param config_dir Path to a the configuration directory
      /// @return an enum indicating the success or failure of the parse
      [[nodiscard]] std::pair<ConfigParseStatus, Mechanism> ReadAndParse(const std::filesystem::path &config_dir)
      {
        return this->Parse(config_dir);
      }
    };
  }
}
