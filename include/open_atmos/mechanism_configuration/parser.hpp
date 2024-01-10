// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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
      InvalidCAMPFilePath,
      NoConfigFilesFound,
      CAMPFilesSectionNotFound,
      CAMPDataSectionNotFound,
      InvalidSpecies,
      InvalidMechanism,
      ObjectTypeNotFound,
      RequiredKeyNotFound,
      ContainsNonStandardKey,
      MutuallyExclusiveOption
    };

    constexpr double MolesM3ToMoleculesCm3 = 1.0e-6 * 6.02214076e23;

    inline std::string configParseStatusToString(const ConfigParseStatus &status)
    {
      switch (status)
      {
      case ConfigParseStatus::Success:
        return "Success";
      case ConfigParseStatus::None:
        return "None";
      case ConfigParseStatus::InvalidKey:
        return "InvalidKey";
      case ConfigParseStatus::UnknownKey:
        return "UnknownKey";
      case ConfigParseStatus::InvalidCAMPFilePath:
        return "InvalidCAMPFilePath";
      case ConfigParseStatus::NoConfigFilesFound:
        return "NoConfigFilesFound";
      case ConfigParseStatus::CAMPFilesSectionNotFound:
        return "CAMPFilesSectionNotFound";
      case ConfigParseStatus::CAMPDataSectionNotFound:
        return "CAMPDataSectionNotFound";
      case ConfigParseStatus::InvalidSpecies:
        return "InvalidSpecies";
      case ConfigParseStatus::InvalidMechanism:
        return "InvalidMechanism";
      case ConfigParseStatus::ObjectTypeNotFound:
        return "ObjectTypeNotFound";
      case ConfigParseStatus::RequiredKeyNotFound:
        return "RequiredKeyNotFound";
      case ConfigParseStatus::ContainsNonStandardKey:
        return "ContainsNonStandardKey";
      case ConfigParseStatus::MutuallyExclusiveOption:
        return "MutuallyExclusiveOption";
      default:
        return "Unknown";
      }
    }

    class JsonReaderPolicy
    {
      using json = nlohmann::json;

    public:

      /// @brief Parse configures
      /// @param config_path Path to a the CAMP configuration directory or file
      /// @return True for successful parsing
      ConfigParseStatus Parse(const std::filesystem::path &config_path);

    private:
    };

    /// @brief Public interface to read and parse config
    template <class ConfigTypePolicy = JsonReaderPolicy>
    class ConfigurationReader : public ConfigTypePolicy
    {
    public:
      /// @brief Reads and parses configures
      /// @param config_dir Path to a the configuration directory
      /// @return an enum indicating the success or failure of the parse
      [[nodiscard]] ConfigParseStatus ReadAndParse(const std::filesystem::path &config_dir);
      {
        return this->Parse(config_dir);
      }
    };
  }
}
