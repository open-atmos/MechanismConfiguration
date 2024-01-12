// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    using nlohmann::json;

    std::string configParseStatusToString(const ConfigParseStatus& status)
    {
      switch (status)
      {
        case ConfigParseStatus::Success: return "Success";
        case ConfigParseStatus::None: return "None";
        case ConfigParseStatus::InvalidKey: return "InvalidKey";
        case ConfigParseStatus::UnknownKey: return "UnknownKey";
        case ConfigParseStatus::InvalidFilePath: return "InvalidFilePath";
        case ConfigParseStatus::ObjectTypeNotFound: return "ObjectTypeNotFound";
        case ConfigParseStatus::RequiredKeyNotFound: return "RequiredKeyNotFound";
        case ConfigParseStatus::MutuallyExclusiveOption: return "MutuallyExclusiveOption";
        case ConfigParseStatus::DuplicateSpeciesDetected: return "DuplicateSpeciesDetected";
        default: return "Unknown";
      }
    }

    std::vector<std::string>
    GetComments(const json& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys

      std::vector<std::string> sorted_object_keys;
      for (auto& [key, value] : object.items())
        sorted_object_keys.push_back(key);

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      return remaining;
    }

    /// @brief Search for nonstandard keys. Only nonstandard keys starting with __ are allowed. Others are considered typos
    /// @param object the object whose keys need to be validated
    /// @param required_keys The required keys
    /// @param optional_keys The optional keys
    /// @return true if only standard keys are found
    ConfigParseStatus ValidateSchema(const json& object, const std::vector<std::string>& required_keys, const std::vector<std::string>& optional_keys)
    {
      // standard keys are:
      // those in required keys
      // those in optional keys
      // starting with __
      // anything else is reported as an error so that typos are caught, specifically for optional keys

      // debug statement
      // std::cout << "ValidateSchema object " << object.dump(4) << std::endl;

      if (!object.empty() && object.begin().value().is_null())
      {
        return ConfigParseStatus::Success;
      }

      std::vector<std::string> sorted_object_keys;
      for (auto& [key, value] : object.items())
        sorted_object_keys.push_back(key);

      auto sorted_required_keys = required_keys;
      auto sorted_optional_keys = optional_keys;
      std::sort(sorted_object_keys.begin(), sorted_object_keys.end());
      std::sort(sorted_required_keys.begin(), sorted_required_keys.end());
      std::sort(sorted_optional_keys.begin(), sorted_optional_keys.end());

      // get the difference between the object keys and those required
      // what's left should be the optional keys and valid comments
      std::vector<std::string> difference;
      std::set_difference(
          sorted_object_keys.begin(),
          sorted_object_keys.end(),
          sorted_required_keys.begin(),
          sorted_required_keys.end(),
          std::back_inserter(difference));

      // check that the number of keys remaining is exactly equal to the expected number of required keys
      if (difference.size() != (sorted_object_keys.size() - required_keys.size()))
      {
        std::vector<std::string> missing_keys;
        std::set_difference(
            sorted_required_keys.begin(),
            sorted_required_keys.end(),
            sorted_object_keys.begin(),
            sorted_object_keys.end(),
            std::back_inserter(missing_keys));
        for (auto& key : missing_keys)
          std::cerr << "Missing required key '" << key << "' in object: " << object << std::endl;

        return ConfigParseStatus::RequiredKeyNotFound;
      }

      std::vector<std::string> remaining;
      std::set_difference(
          difference.begin(), difference.end(), sorted_optional_keys.begin(), sorted_optional_keys.end(), std::back_inserter(remaining));

      // now, anything left must be standard comment starting with __
      for (auto& key : remaining)
      {
        if (!key.starts_with("__"))
        {
          std::cerr << "Non-standard key '" << key << "' found in object" << object << std::endl;

          return ConfigParseStatus::InvalidKey;
        }
      }
      return ConfigParseStatus::Success;
    }

    std::pair<ConfigParseStatus, std::vector<types::Species>> ParseSpecies(const json& objects)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      std::vector<types::Species> all_species;

      for (const auto& object : objects)
      {
        types::Species species;
        status = ValidateSchema(object, validation::species.required_keys, validation::species.optional_keys);
        if (status != ConfigParseStatus::Success)
        {
          break;
        }

        std::string name = object[validation::keys.name].get<std::string>();
        std::string phase = object[validation::keys.phase].get<std::string>();

        std::map<std::string, double> numerical_properties{};
        for (const auto& key : validation::species.optional_keys)
        {
          if (object.contains(key))
          {
            double val = object[key].get<double>();
            numerical_properties[key] = val;
          }
        }

        auto comments = GetComments(object, validation::species.required_keys, validation::species.optional_keys);

        std::unordered_map<std::string, std::string> unknown_properties;
        for (const auto& key : comments)
        {
          std::string val = object[key].dump();
          unknown_properties[key] = val;
        }

        species.name = name;
        species.phase = phase;
        species.optional_numerical_properties = numerical_properties;
        species.unknown_properties = unknown_properties;

        all_species.push_back(species);
      }

      // check for duplicate species
      for (size_t i = 0; i < all_species.size(); ++i)
      {
        for (size_t j = i + 1; j < all_species.size(); ++j)
        {
          if (all_species[i].name == all_species[j].name)
          {
            status = ConfigParseStatus::DuplicateSpeciesDetected;
          }
          break;
        }
        if (status != ConfigParseStatus::Success)
          break;
      }

      return { status, all_species };
    }

    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const std::string& file_path)
    {
      return JsonParser::Parse(std::filesystem::path(file_path));
    }

    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const std::filesystem::path& file_path)
    {
      ConfigParseStatus status;

      if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path))
      {
        status = ConfigParseStatus::InvalidFilePath;
        std::string msg = configParseStatusToString(status);
        std::cerr << msg << std::endl;
        return { status, types::Mechanism() };
      }

      json config = json::parse(std::ifstream(file_path));

      return JsonParser::Parse(config);
    }

    std::pair<ConfigParseStatus, types::Mechanism> JsonParser::Parse(const nlohmann::json& object)
    {
      ConfigParseStatus status;
      types::Mechanism mechanism;

      status = ValidateSchema(object, validation::configuration.required_keys, validation::configuration.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Invalid top level configuration." << std::endl;
        return { status, mechanism };
      }

      std::string version = object[validation::keys.version].get<std::string>();

      if (version != getVersionString())
      {
        status = ConfigParseStatus::InvalidVersion;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] This parser supports version " << getVersionString() << " and you requested version " << version
                  << ". Please download the appropriate version of the parser or switch to the supported format's version." << std::endl;
      }

      std::string name = object[validation::keys.name].get<std::string>();
      mechanism.name = name;

      // parse all of the species at once
      auto species_parsing = ParseSpecies(object["species"]);

      if (species_parsing.first != ConfigParseStatus::Success)
      {
        status = species_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the species." << std::endl;
      }

      mechanism.species = species_parsing.second;

      return { status, mechanism };
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
