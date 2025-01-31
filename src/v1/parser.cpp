// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>
#include <mechanism_configuration/load_node.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    ParserResult<types::Mechanism> Parser::Parse(const std::filesystem::path& config_path)
    {
      YAML::Node object = LoadNode(config_path);
      ConfigParseStatus status;
      ParserResult<types::Mechanism> result;
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();

      status = ValidateSchema(object, validation::mechanism.required_keys, validation::mechanism.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        result.errors.push_back({ status, "Invalid top level configuration." });
        return result;
      }

      Version version = Version(object[validation::keys.version].as<std::string>());

      if (version.major != 1)
      {
        status = ConfigParseStatus::InvalidVersion;
        result.errors.push_back({ status, "Invalid version." });
      }

      std::string name = object[validation::keys.name].as<std::string>();
      mechanism->name = name;

      auto species_parsing = ParseSpecies(object[validation::keys.species]);

      if (species_parsing.first != ConfigParseStatus::Success)
      {
        status = species_parsing.first;
        result.errors.push_back({ status, "Failed to parse the species." });
      }

      auto phases_parsing = ParsePhases(object[validation::keys.phases], species_parsing.second);

      if (phases_parsing.first != ConfigParseStatus::Success)
      {
        status = phases_parsing.first;
        result.errors.push_back({ status, "Failed to parse the phases." });
      }

      auto reactions_parsing = ParseReactions(object[validation::keys.reactions], species_parsing.second, phases_parsing.second);

      if (reactions_parsing.first != ConfigParseStatus::Success)
      {
        status = reactions_parsing.first;
        result.errors.push_back({ status, "Failed to parse the reactions." });
      }

      mechanism->species = species_parsing.second;
      mechanism->phases = phases_parsing.second;
      mechanism->reactions = reactions_parsing.second;

      result.mechanism = std::move(mechanism);
      return result;
    }
  }  // namespace v1
}  // namespace mechanism_configuration