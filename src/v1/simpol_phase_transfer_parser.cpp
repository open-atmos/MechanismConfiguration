#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    ConfigParseStatus SimpolPhaseTransferParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::SimpolPhaseTransfer simpol_phase_transfer;

      status = ValidateSchema(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        std::string gas_phase_species = object[validation::keys.gas_phase_species].as<std::string>();
        std::string aerosol_phase_species = object[validation::keys.aerosol_phase_species].as<std::string>();

        if (object[validation::keys.name])
        {
          simpol_phase_transfer.name = object[validation::keys.name].as<std::string>();
        }

        std::vector<std::string> requested_species{ gas_phase_species, aerosol_phase_species };
        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
        auto aerosol_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const auto& phase) { return phase.name == aerosol_phase; });
        if (status == ConfigParseStatus::Success && aerosol_it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }
        else
        {
          auto phase = *aerosol_it;
          auto spec_it = std::find_if(
              phase.species.begin(),
              phase.species.end(),
              [&aerosol_phase_species](const std::string& species) { return species == aerosol_phase_species; });
          if (spec_it == phase.species.end())
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }
        }

        std::string gas_phase = object[validation::keys.gas_phase].as<std::string>();
        auto gas_it =
            std::find_if(existing_phases.begin(), existing_phases.end(), [&gas_phase](const auto& phase) { return phase.name == gas_phase; });
        if (status == ConfigParseStatus::Success && gas_it == existing_phases.end())
        {
          status = ConfigParseStatus::UnknownPhase;
        }
        else
        {
          auto phase = *gas_it;
          auto spec_it = std::find_if(
              phase.species.begin(), phase.species.end(), [&gas_phase_species](const std::string& species) { return species == gas_phase_species; });
          if (spec_it == phase.species.end())
          {
            status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
          }
        }

        if (object[validation::keys.B] && object[validation::keys.B].IsSequence() && object[validation::keys.B].size() == 4)
        {
          for (size_t i = 0; i < 4; ++i)
          {
            simpol_phase_transfer.B[i] = object[validation::keys.B][i].as<double>();
          }
        }

        simpol_phase_transfer.gas_phase = gas_phase;
        types::ReactionComponent gas_component;
        gas_component.species_name = gas_phase_species;
        simpol_phase_transfer.gas_phase_species = gas_component;
        simpol_phase_transfer.aerosol_phase = aerosol_phase;
        types::ReactionComponent aerosol_component;
        aerosol_component.species_name = aerosol_phase_species;
        simpol_phase_transfer.aerosol_phase_species = aerosol_component;
        simpol_phase_transfer.unknown_properties =
            GetComments(object, validation::simpol_phase_transfer.required_keys, validation::simpol_phase_transfer.optional_keys);
        reactions.simpol_phase_transfer.push_back(simpol_phase_transfer);
      }

      return status;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
