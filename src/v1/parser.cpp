// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/validation.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    std::pair<ConfigParseStatus, types::Reactions>
    ParseReactions(const YAML::Node& objects, const std::vector<types::Species>& existing_species, const std::vector<types::Phase>& existing_phases)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::Reactions reactions;

      std::map<std::string, std::unique_ptr<IReactionParser>> parsers;
      parsers[validation::keys.Arrhenius_key] = std::make_unique<ArrheniusParser>();
      parsers[validation::keys.HenrysLaw_key] = std::make_unique<HenrysLawParser>();
      parsers[validation::keys.WetDeposition_key] = std::make_unique<WetDepositionParser>();
      parsers[validation::keys.AqueousPhaseEquilibrium_key] = std::make_unique<AqueousEquilibriumParser>();
      parsers[validation::keys.SimpolPhaseTransfer_key] = std::make_unique<SimpolPhaseTransferParser>();
      parsers[validation::keys.FirstOrderLoss_key] = std::make_unique<FirstOrderLossParser>();
      parsers[validation::keys.Emission_key] = std::make_unique<EmissionParser>();
      parsers[validation::keys.CondensedPhasePhotolysis_key] = std::make_unique<CondensedPhasePhotolysisParser>();
      parsers[validation::keys.Photolysis_key] = std::make_unique<PhotolysisParser>();
      parsers[validation::keys.Surface_key] = std::make_unique<SurfaceParser>();
      parsers[validation::keys.Tunneling_key] = std::make_unique<TunnelingParser>();
      parsers[validation::keys.Branched_key] = std::make_unique<BranchedParser>();
      parsers[validation::keys.Troe_key] = std::make_unique<TroeParser>();
      parsers[validation::keys.CondensedPhaseArrhenius_key] = std::make_unique<CondensedPhaseArrheniusParser>();

      for (const auto& object : objects)
      {
        std::string type = object[validation::keys.type].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_status = it->second->parse(object, existing_species, existing_phases, reactions);
          status = parse_status;
          if (status != ConfigParseStatus::Success)
          {
            break;
          }
        }
        else
        {
          const std::string& msg = "Unknown type: " + type;
          throw std::runtime_error(msg);
        }
      }

      return { status, reactions };
    }

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