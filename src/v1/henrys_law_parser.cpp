#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    Errors HenrysLawParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      Errors errors;
      types::HenrysLaw henrys_law;

      auto validate = ValidateSchema(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
      errors.insert(errors.end(), validate.begin(), validate.end());
      if (validate.empty())
      {
        std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
        std::string gas_phase_species = object[validation::keys.gas_phase_species].as<std::string>();
        std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
        std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].as<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].as<std::string>();

        if (object[validation::keys.name])
        {
          henrys_law.name = object[validation::keys.name].as<std::string>();
        }

        std::vector<std::string> requested_species;
        requested_species.push_back(gas_phase_species);
        requested_species.push_back(aerosol_phase_species);
        requested_species.push_back(aerosol_phase_water);

        std::vector<std::string> requested_aerosol_species;
        requested_aerosol_species.push_back(aerosol_phase_species);
        requested_aerosol_species.push_back(aerosol_phase_water);

        if (RequiresUnknownSpecies(requested_species, existing_species))
        {
          std::string line = std::to_string(object.Mark().line + 1);
          std::string column = std::to_string(object.Mark().column + 1);
          errors.push_back({ ConfigParseStatus::ReactionRequiresUnknownSpecies, line + ":" + column + ": Reaction requires unknown species" });
        }

        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (it == existing_phases.end())
        {
          std::string line = std::to_string(object[validation::keys.gas_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.gas_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + gas_phase });
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (RequiresUnknownSpecies(requested_aerosol_species, aerosol_phase_species))
          {
            std::string line = std::to_string(object.Mark().line + 1);
            std::string column = std::to_string(object.Mark().column + 1);
            errors.push_back({ ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase,
                               line + ":" + column + ": Requested aerosol species not included in aerosol phase" });
          }
        }
        else
        {
          std::string line = std::to_string(object[validation::keys.aerosol_phase].Mark().line + 1);
          std::string column = std::to_string(object[validation::keys.aerosol_phase].Mark().column + 1);
          errors.push_back({ ConfigParseStatus::UnknownPhase, line + ":" + column + ": Unknown phase: " + aerosol_phase });
        }

        henrys_law.gas_phase = gas_phase;
        henrys_law.gas_phase_species = gas_phase_species;
        henrys_law.aerosol_phase = aerosol_phase;
        henrys_law.aerosol_phase_species = aerosol_phase_species;
        henrys_law.aerosol_phase_water = aerosol_phase_water;
        henrys_law.unknown_properties = GetComments(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
        reactions.henrys_law.push_back(henrys_law);
      }

      return errors;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
