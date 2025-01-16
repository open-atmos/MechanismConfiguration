// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/validation.hpp>
#include <open_atmos/mechanism_configuration/version.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
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

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const std::string& file_path)
    {
      return Parser::Parse(std::filesystem::path(file_path));
    }

    /// @brief Parse a mechanism
    /// @param file_path a location on the hard drive containing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const std::filesystem::path& file_path)
    {
      ConfigParseStatus status;

      if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path))
      {
        status = ConfigParseStatus::InvalidFilePath;
        std::string msg = configParseStatusToString(status);
        std::cerr << msg << std::endl;
        return { status, types::Mechanism() };
      }

      YAML::Node config = YAML::LoadFile(file_path.string());

      return Parser::Parse(config);
    }

    /// @brief Parse a mechanism
    /// @param node A yaml object representing a mechanism
    /// @return A pair containing the parsing status and a mechanism
    std::pair<ConfigParseStatus, types::Mechanism> Parser::Parse(const YAML::Node& object)
    {
      ConfigParseStatus status;
      types::Mechanism mechanism;

      status = ValidateSchema(object, validation::mechanism.required_keys, validation::mechanism.optional_keys);

      if (status != ConfigParseStatus::Success)
      {
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] Invalid top level configuration." << std::endl;
        return { status, mechanism };
      }

      std::string version = object[validation::keys.version].as<std::string>();

      if (version != getVersionString())
      {
        status = ConfigParseStatus::InvalidVersion;
        std::string msg = configParseStatusToString(status);
        std::cerr << "[" << msg << "] This parser supports version " << getVersionString() << " and you requested version " << version
                  << ". Please download the appropriate version of the parser or switch to the supported format's version." << std::endl;
      }

      std::string name = object[validation::keys.name].as<std::string>();
      mechanism.name = name;

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

      mechanism.species = species_parsing.second;
      mechanism.phases = phases_parsing.second;
      mechanism.reactions = reactions_parsing.second;

      return { status, mechanism };
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos