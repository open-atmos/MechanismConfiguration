// Copyright (C) 2023-2024 National Center for Atmospheric Research, University of Illinois at Urbana-Champaign
//
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/conversions.hpp>
#include <mechanism_configuration/v0/parser.hpp>
#include <mechanism_configuration/v0/parser_types.hpp>
#include <mechanism_configuration/v0/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v0
  {

    using ParserMap = std::map<std::string, std::function<ConfigParseStatus(std::unique_ptr<types::Mechanism>&, const YAML::Node&)>>;

    ConfigParseStatus run_parsers(const ParserMap& parsers, std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      for (const auto& element : object)
      {
        std::string type = element[validation::TYPE].as<std::string>();
        auto it = parsers.find(type);
        if (it != parsers.end())
        {
          auto parse_status = it->second(mechanism, element);
          if (parse_status != ConfigParseStatus::Success)
          {
            status = parse_status;
          }
        }
        else
        {
          const std::string& msg = "Unknown type: " + type;
          throw std::runtime_error(msg);
        }
      }
      return status;
    }

    ConfigParseStatus ParseMechanism(const ParserMap& parsers, std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object) {
      ConfigParseStatus status = ConfigParseStatus::Success;
      auto required = { validation::NAME, validation::REACTIONS, validation::TYPE };

      status = ValidateSchema(object, required, {});
      if (status == ConfigParseStatus::Success)
      {
        mechanism->name = object[validation::NAME].as<std::string>();
        status = run_parsers(parsers, mechanism, object[validation::REACTIONS]);
      }

      return status;
    }


    std::optional<std::unique_ptr<GlobalMechanism>> Parser::TryParse(const std::filesystem::path& config_path)
    {
      ConfigParseStatus status;
      std::unique_ptr<types::Mechanism> mechanism = std::make_unique<types::Mechanism>();

      // Look for CAMP config path
      if (!std::filesystem::exists(config_path))
      {
        std::cerr << "File does not exist: " << config_path << std::endl;
        return std::nullopt;
      }

      std::filesystem::path config_dir;
      std::filesystem::path config_file;

      if (std::filesystem::is_directory(config_path))
      {
        // If config path is a directory, use default config file name
        config_dir = config_path;
        if (std::filesystem::exists(config_dir / DEFAULT_CONFIG_FILE_YAML))
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_YAML;
        }
        else
        {
          config_file = config_dir / DEFAULT_CONFIG_FILE_JSON;
        }
      }
      else
      {
        // Extract configuration dir from configuration file path
        config_dir = config_path.parent_path();
        config_file = config_path;
      }

      // Load the CAMP file list YAML
      YAML::Node camp_data = YAML::LoadFile(config_file.string());
      if (!camp_data[CAMP_FILES])
      {
        std::cerr << "CAMP files not found in: " << config_file << std::endl;
        return std::nullopt;
      }

      // Build a list of individual CAMP config files
      std::vector<std::filesystem::path> camp_files;
      for (const auto& element : camp_data[CAMP_FILES])
      {
        std::filesystem::path camp_file = config_dir / element.as<std::string>();
        if (!std::filesystem::exists(camp_file))
        {
          std::cerr << "CAMP file not found: " << camp_file << std::endl;
          return std::nullopt;
        }
        camp_files.push_back(camp_file);
      }

      // No config files found
      if (camp_files.size() < 1)
      {
        std::cerr << "No CAMP files found in: " << config_file << std::endl;
        return std::nullopt;
      }

      ParserMap parsers;

      parsers["CHEM_SPEC"] = ParseChemicalSpecies;
      parsers["RELATIVE_TOLERANCE"] = ParseRelativeTolerance;
      parsers["PHOTOLYSIS"] = PhotolysisParser;
      parsers["EMISSION"] = EmissionParser;
      parsers["FIRST_ORDER_LOSS"] = FirstOrderLossParser;
      parsers["ARRHENIUS"] = ArrheniusParser;
      parsers["TROE"] = TroeParser;
      parsers["TERNARY_CHEMICAL_ACTIVATION"] = TernaryChemicalActivationParser;
      parsers["BRANCHED"] = BranchedParser;
      parsers["WENNBERG_NO_RO2"] = BranchedParser;
      parsers["TUNNELING"] = TunnelingParser;
      parsers["WENNBERG_TUNNELING"] = TunnelingParser;
      parsers["SURFACE"] = SurfaceParser;
      parsers["USER_DEFINED"] = UserDefinedParser;

      std::function<ConfigParseStatus(std::unique_ptr<types::Mechanism>&, const YAML::Node&)> ParseMechanismArray = [&](std::unique_ptr<types::Mechanism>& mechanism, const YAML::Node& object) {
        return ParseMechanism(parsers, mechanism, object);
      };

      parsers["MECHANISM"] = ParseMechanismArray;

      // Iterate CAMP file list and form CAMP data object arrays
      for (const auto& camp_file : camp_files)
      {
        YAML::Node config_subset = YAML::LoadFile(camp_file.string());

        status = run_parsers(parsers, mechanism, config_subset[CAMP_DATA]);
      }

      // all species in version 0 are in the gas phase
      types::Phase gas_phase;
      gas_phase.name = "GAS";
      for (auto& species : mechanism->species)
      {
        gas_phase.species.push_back(species.name);
      }

      return std::unique_ptr<GlobalMechanism>(std::move(mechanism));
    }
  }  // namespace v0
}  // namespace mechanism_configuration