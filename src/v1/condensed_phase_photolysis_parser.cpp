#include <mechanism_configuration/constants.hpp>
#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/parser_types.hpp>
#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    ConfigParseStatus CondensedPhasePhotolysisParser::parse(
        const YAML::Node& object,
        const std::vector<types::Species>& existing_species,
        const std::vector<types::Phase>& existing_phases,
        types::Reactions& reactions)
    {
      ConfigParseStatus status = ConfigParseStatus::Success;
      types::CondensedPhasePhotolysis condensed_phase_photolysis;

      status = ValidateSchema(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);
      if (status == ConfigParseStatus::Success)
      {
        auto products = ParseReactantsOrProducts(validation::keys.products, object, status);
        auto reactants = ParseReactantsOrProducts(validation::keys.reactants, object, status);

        if (object[validation::keys.scaling_factor])
        {
          condensed_phase_photolysis.scaling_factor_ = object[validation::keys.scaling_factor].as<double>();
        }

        if (object[validation::keys.name])
        {
          condensed_phase_photolysis.name = object[validation::keys.name].as<std::string>();
        }

        std::string aerosol_phase = object[validation::keys.aerosol_phase].as<std::string>();
        std::string aerosol_phase_water = object[validation::keys.aerosol_phase_water].as<std::string>();

        std::vector<std::string> requested_species;
        for (const auto& spec : products)
        {
          requested_species.push_back(spec.species_name);
        }
        for (const auto& spec : reactants)
        {
          requested_species.push_back(spec.species_name);
        }
        requested_species.push_back(aerosol_phase_water);

        if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, existing_species))
        {
          status = ConfigParseStatus::ReactionRequiresUnknownSpecies;
        }

        if (status == ConfigParseStatus::Success && reactants.size() > 1)
        {
          status = ConfigParseStatus::TooManyReactionComponents;
        }

        auto phase_it = std::find_if(
            existing_phases.begin(), existing_phases.end(), [&aerosol_phase](const types::Phase& phase) { return phase.name == aerosol_phase; });

        if (phase_it != existing_phases.end())
        {
          std::vector<std::string> aerosol_phase_species = { (*phase_it).species.begin(), (*phase_it).species.end() };
          if (status == ConfigParseStatus::Success && RequiresUnknownSpecies(requested_species, aerosol_phase_species))
          {
            status = ConfigParseStatus::RequestedAerosolSpeciesNotIncludedInAerosolPhase;
          }
        }
        else
        {
          status = ConfigParseStatus::UnknownPhase;
        }

        condensed_phase_photolysis.aerosol_phase = aerosol_phase;
        condensed_phase_photolysis.aerosol_phase_water = aerosol_phase_water;
        condensed_phase_photolysis.products = products;
        condensed_phase_photolysis.reactants = reactants;
        condensed_phase_photolysis.unknown_properties =
            GetComments(object, validation::condensed_phase_photolysis.required_keys, validation::photolysis.optional_keys);
        reactions.condensed_phase_photolysis.push_back(condensed_phase_photolysis);
      }

      return status;
    }
  }  // namespace v1
}  // namespace mechanism_configuration
