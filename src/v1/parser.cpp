// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/v1/utils.hpp>


namespace mechanism_configuration
{
  namespace v1
  {
    std::optional<std::unique_ptr<GlobalMechanism>> Parser::TryParse(const YAML::Node& object)
    {
      ConfigParseStatus status;
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();

      status = ValidateSchema(object, validation::mechanism.required_keys, validation::mechanism.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Invalid top level configuration." << std::endl;
        return std::nullopt;
      }

      Version version = Version(object[validation::keys.version].as<std::string>());

      if (version.major != 1)
      {
        status = ConfigParseStatus::InvalidVersion;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] This parser supports version 1.x and you requested version " << version.to_string()
                  << ". Please download the appropriate version of the parser or switch to the supported format's version." << std::endl;
      }

      std::string name = object[validation::keys.name].as<std::string>();
      mechanism->name = name;

      auto species_parsing = ParseSpecies(object[validation::keys.species]);

      if (species_parsing.first != ConfigParseStatus::Success)
      {
        status = species_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the species." << std::endl;
      }

      auto phases_parsing = ParsePhases(object[validation::keys.phases], species_parsing.second);

      if (phases_parsing.first != ConfigParseStatus::Success)
      {
        status = phases_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the phases." << std::endl;
      }

      auto reactions_parsing = ParseReactions(object[validation::keys.reactions], species_parsing.second, phases_parsing.second);

      if (reactions_parsing.first != ConfigParseStatus::Success)
      {
        status = reactions_parsing.first;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Failed to parse the reactions." << std::endl;
      }

      mechanism->species = species_parsing.second;
      mechanism->phases = phases_parsing.second;
      mechanism->reactions = reactions_parsing.second;

      if (status == ConfigParseStatus::Success)
      {
        return std::unique_ptr<GlobalMechanism>(std::move(mechanism));
      }
      else
      {
        return std::nullopt;
      }
    }
  }  // namespace v1
}  // namespace mechanism_configuration