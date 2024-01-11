// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <open_atmos/mechanism_configuration/parser.hpp>
#include <nlohmann/json.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {

    // explicit template instanatiation
    template class ConfigurationReader<JsonReaderPolicy>;

    std::string configParseStatusToString(const ConfigParseStatus &status)
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

    std::pair<ConfigParseStatus, Mechanism> JsonReaderPolicy::Parse(const std::filesystem::path &config_path) {
      return {ConfigParseStatus::Success, Mechanism()};
    }
  }
}