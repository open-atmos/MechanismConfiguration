#include <open_atmos/constants.hpp>
#include <open_atmos/mechanism_configuration/parser.hpp>
#include <open_atmos/mechanism_configuration/parser_types.hpp>
#include <open_atmos/mechanism_configuration/utils.hpp>

namespace open_atmos
{
  namespace mechanism_configuration
  {
    ConfigParseStatus HenrysLawParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        open_atmos::types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::HenrysLaw henrys_law;

      status = ValidateSchema(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
      if (status == ConfigParseStatus::Success)
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

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        auto it = std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_aerosol_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        henrys_law.gas_phase = gas_phase;
        henrys_law.gas_phase_species = gas_phase_species;
        henrys_law.aerosol_phase = aerosol_phase;
        henrys_law.aerosol_phase_species = aerosol_phase_species;
        henrys_law.aerosol_phase_water = aerosol_phase_water;
        henrys_law.unknown_properties = GetComments(object, validation::henrys_law.required_keys, validation::henrys_law.optional_keys);
        reactions.henrys_law.push_back(henrys_law);
      }

      return status;
    }
  }  // namespace mechanism_configuration
}  // namespace open_atmos
